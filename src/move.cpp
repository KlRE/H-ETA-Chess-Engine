#include "move.h"
#include "attacks.h"
#include "utility.h"
#include "board.h"


#include <iostream>

using namespace std;


ostream &operator<<(ostream &os, Move &m) {
  os << printSq(m.from()) << printSq(m.to()) << " Type: " << m.flags() << "\n";
  return os;
}

ostream &operator<<(ostream &os, Move *m) {
  os << printSq(m->from()) << printSq(m->to()) << " Type: " << m->flags() << "\n";
  return os;
}


Move *Board::generateKingM(Move *list, const uint64_t &attacked, const uint64_t &OpPieces, Square sq) {
  uint64_t Ksq = KingAttack(sq) & ~attacked;
  //drawB(Ksq);

  vector<Square> KsqV = getPos(Ksq);

  for (int i: KsqV) {
    if (1ull << i & OpPieces)  // King captures Piece
      *list++ = Move(sq, i, CAPTURE);
    else
      *list++ = Move(sq, i, QUIET);
  }

  return list;
}


Move *Board::generateCastles(Move *list, const uint64_t &attacked) {
  uint64_t att, sq1, sq2, sq3;
  if (SideToMove == White)
    att = 0x200000000000000, sq1 = e1, sq2 = c1, sq3 = g1;  //0x200000000000000 = b1
  else
    att = 2, sq1 = e8, sq2 = c8, sq3 = g8;  // 0x2 = b8

  //                                logical ! on integer means 0 => true else => false  
  bool QueenS = castle[Queenside][SideToMove] && !(getCastleBbs(Queenside, SideToMove) & attacked) && (att & ~getP()); //todo why
  bool KingS = castle[Kingside][SideToMove] && !(getCastleBbs(Kingside, SideToMove) & attacked);

  if (QueenS) *list++ = Move(sq1, sq2, OOO);
  if (KingS) *list++ = Move(sq1, sq3, OO);

  return list;
}

Move *Board::generatePawnM(Move *list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask,
                           uint64_t pos, int kingSq) {
  uint64_t promLine = SideToMove == White ? getLines(a7, h7) : getLines(a2, h2);
  while (pos) {
    Square from = pop_lsb(pos);
    uint64_t doublePushMask = 0ull;
    uint64_t quiet = PawnMove(from, SideToMove, getP(), doublePushMask) & pushMask;
    uint64_t cap = PawnAttack(from, SideToMove) & captureMask;
    uint64_t pinnedLine = fullBb();

    // drawB(promLine);
    // drawB(cap);

    if (toBb(from) & pinned) {
      quiet &= getLines(from, kingSq);
      cap &= getLines(from, kingSq);
      pinnedLine = getLines(from, kingSq);
    }

    //quiet |= doublePushMask;
    while (quiet) {
      Square to = pop_lsb(quiet);
      if (toBb(from) & promLine) {
        *list++ = Move(from, to, PR_KNIGHT);
        *list++ = Move(from, to, PR_BISHOP);
        *list++ = Move(from, to, PR_ROOK);
        *list++ = Move(from, to, PR_QUEEN);
      }
      else
        *list++ = Move(from, to, QUIET);
    }

    while(doublePushMask) {
      //drawB(doublePushMask);
      Square to = pop_lsb(doublePushMask);
      *list++ = Move(from, to, DOUBLE_PUSH);
    }

    while (cap) {
      Square to = pop_lsb(cap);
      if (toBb(from) & promLine) {
        *list++ = Move(from, to, PC_KNIGHT);
        *list++ = Move(from, to, PC_BISHOP);
        *list++ = Move(from, to, PC_ROOK);
        *list++ = Move(from, to, PC_QUEEN);
      }
      else
        *list++ = Move(from, to, CAPTURE);
    }

    // en passant
    if (PawnAttack(from, SideToMove) & epSqBb) {
      uint64_t epLine;   // determining where the square of captured pawn ist
      if (SideToMove == White)
        epLine = getLines(a5, h5);
      else
        epLine = getLines(a4, h4);

      uint64_t capturedSqBb = epLine & mRook(lsb(epSqBb), 0ull); //overlap of these two masks is this Square
      if (((epSqBb & pushMask & pinnedLine)    //if the pawn ends up on a moveable square
           || (capturedSqBb & captureMask & pinnedLine)))  // if pawn captures capturable square
      {
        //checking if the king is in check (from rook or queen) after ep on the horizontal axis
        // function: look if king is in check after taking away these two pawns by calc mRook from the king sq
        //   lookup piece on square by logical and with rook and queen bitboard of opponent
        //also checking diagonal line, after capturing a pawn
        uint64_t occwoPawn = getP() & ~(toBb(from) | capturedSqBb);
        uint64_t kingCheck = (epLine & mRook(kingSq, occwoPawn) & (getP(!SideToMove, Rook) | getP(!SideToMove, Queen)))
                             | (getLines(kingSq, lsb(capturedSqBb)) & mBishop(kingSq, occwoPawn) &
                                (getP(!SideToMove, Bishop) | getP(!SideToMove, Queen)));
        //drawB(kingCheck);

        if (!kingCheck)
          *list++ = Move(from, lsb(epSqBb), EN_PASSANT);
      }

    }

  }
  return list;
}

Move *Board::generateRookM(Move *list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask,
                           uint64_t pos, int kingSq) {
  while (pos) {
    Square from = pop_lsb(pos);
    uint64_t att;                          //maybe summarize to one with att=mrook and if(pinned) s. pawn

    if (toBb(from) & pinned)
      att = mRook(from, getP()) & getLines(from, kingSq);
    else
      att = mRook(from, getP());

    uint64_t quiet = att & pushMask;
    uint64_t cap = att & captureMask;

    while (quiet) {
      Square to = pop_lsb(quiet);
      *list++ = Move(from, to, QUIET);
    }
    while (cap) {
      Square to = pop_lsb(cap);
      *list++ = Move(from, to, CAPTURE);
    }
  }
  return list;
}

Move *Board::generateBishopM(Move *list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask,
                             uint64_t pos, int kingSq) {
  while (pos) {
    Square from = pop_lsb(pos);
    uint64_t att;

    if (toBb(from) & pinned)
      att = mBishop(from, getP()) & getLines(from, kingSq);
    else
      att = mBishop(from, getP());

    uint64_t quiet = att & pushMask;
    uint64_t cap = att & captureMask;

    while (quiet) {
      Square to = pop_lsb(quiet);
      *list++ = Move(from, to, QUIET);
    }
    while (cap) {
      Square to = pop_lsb(cap);
      *list++ = Move(from, to, CAPTURE);
    }
  }
  return list;
}

Move *Board::generateKnightM(Move *list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask,
                             uint64_t pos) {
  while (pos) {
    Square from = pop_lsb(pos);
    uint64_t att;

    if (toBb(from) & pinned)
      continue;
    else
      att = KnightAttack(from);

    uint64_t quiet = att & pushMask;
    uint64_t cap = att & captureMask;

    while (quiet) {
      Square to = pop_lsb(quiet);
      *list++ = Move(from, to, QUIET);
    }
    while (cap) {
      Square to = pop_lsb(cap);
      *list++ = Move(from, to, CAPTURE);
    }
  }
  return list;
}

Move *Board::generateQueenM(Move *list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask,
                            uint64_t pos, int kingSq) {
  list = generateRookM(list, pinned, pushMask, captureMask, pos, kingSq);
  list = generateBishopM(list, pinned, pushMask, captureMask, pos, kingSq);
  return list;
}


Move *Board::generateMoves(Move *list) {
  uint64_t attacked = 0ull;
  uint64_t temp, tempAtt;    // temp bitboard and temp for attacks

  // Opponent Pawn Attacks
  vector<Square> OpPawn = PieceSqs(!SideToMove,
                                   Pawn);   //todo maybe remove the conversion to vector and doing it with while(bitboard) pop_lsb(bitboard) (s. surge-master)
  //  for(Square s:OpPawn)cout<<s<<" ";
  vector<Square> OpRook = PieceSqs(!SideToMove, Rook);
  vector<Square> OpKnight = PieceSqs(!SideToMove, Knight);
  vector<Square> OpBishop = PieceSqs(!SideToMove, Bishop);
  vector<Square> OpQueen = PieceSqs(!SideToMove, Queen);
  Square OpKing = PieceSqs(!SideToMove, King)[0];

  Square OuKing = PieceSqs(SideToMove, King)[0];
  uint64_t allPwoK = getP() & ~(1ull << OuKing); // bitboard with all Pieces except King of Playing side
  // drawB(allPwoK);

  for (int i: OpPawn) attacked |= PawnAttack(i, !SideToMove);
  for (int i: OpRook) attacked |= mRook(i, allPwoK);
  for (int i: OpKnight) attacked |= KnightAttack(i);
  for (int i: OpBishop) attacked |= mBishop(i, allPwoK);
  for (int i: OpQueen) attacked |= mQueen(i, allPwoK);

  attacked |= KingAttack(OpKing);
  attacked |= getP(SideToMove);  //Pieces of opponent are either capturable or attacked by other opp pieces

  //drawB(attacked);

  list = generateKingM(list, attacked, getP(!SideToMove), OuKing);

  uint64_t checkers = attacksTo(OuKing,
                                !SideToMove); //maybe no need to check for queen and rook bc it can be done later with pinning step (s. surge-master)
  //drawB(checkers);

  uint64_t captureMask;
  uint64_t pushMask;

  int num_checkers = popcount(checkers);

  if (num_checkers > 1)
    return list;

  else if (num_checkers == 1) {
    pushMask = getSquaresBetween(lsb(checkers), OuKing);
    captureMask = checkers;
  }

  else {
    captureMask = getP(!SideToMove);
    pushMask = ~getP();
    list = generateCastles(list, attacked);
  }

  uint64_t pinned = sliderBlockers(OuKing, SideToMove);   // pinned pieces


  //Pawn Moves
  list = generatePawnM(list, pinned, pushMask, captureMask, getP(SideToMove, Pawn), OuKing);
  list = generateRookM(list, pinned, pushMask, captureMask, getP(SideToMove, Rook), OuKing);
  list = generateKnightM(list, pinned, pushMask, captureMask, getP(SideToMove, Knight));
  list = generateBishopM(list, pinned, pushMask, captureMask, getP(SideToMove, Bishop), OuKing);
  list = generateQueenM(list, pinned, pushMask, captureMask, getP(SideToMove, Queen), OuKing);

  //drawB(getP(SideToMove, Pawn));

  return list;
}

// plays the move m on the board    TODO: test everything from 000 onward
void Board::play(const Move &m) {
  ++halfMovesAfterStart;
  history[halfMovesAfterStart] = UndoInfo(history[halfMovesAfterStart - 1]);
  if (board[m.from()] == Piece::Pawn || m.flags() & CAPTURE)
      history[halfMovesAfterStart].halfMoves = 0;

  switch(m.flags()) {
    case DOUBLE_PUSH:
      setEpSqFromDoublePush(Square(m.to()));
      history[halfMovesAfterStart].epSq = epSqBb; //TODO: put in genMoves without variable epSqBb
    case QUIET:
      playQuiet(Square(m.from()), Square(m.to()));
      break;

    case OO:
      playQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == White) { // king moved alr, so rook has to move too
        castle[Kingside][White] = false;
        castle[Queenside][White] = false;
        playQuiet(h1, f1);
      }
      else {
        castle[Kingside][Black] = false;
        castle[Queenside][Black] = false;
        playQuiet(h8, f8);
      }
      break;

    case OOO:
      playQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == White) { // king moved alr, so rook has to move too
        castle[Kingside][White] = false;
        castle[Queenside][White] = false;
        playQuiet(a1, d1);
      }
      else {
        castle[Kingside][Black] = false;
        castle[Queenside][Black] = false;
        playQuiet(a8, d8);
      }
      break;

    case CAPTURE:
      history[halfMovesAfterStart].captured = Piece(board[m.to()]);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      playQuiet(Square(m.from()), Square(m.to()));
      break;

    case EN_PASSANT:
      if(SideToMove == White) {
        history[halfMovesAfterStart].captured = Piece(board[m.to() + 8]);
        removePiece(Square(m.to() + 8), Piece(board[m.to() + 8]), !SideToMove);   // the same as going one behind (e.g. e6 -> e5) from whites view
      }
      else {
        history[halfMovesAfterStart].captured = Piece(board[m.to() - 8]);
        removePiece(Square(m.to() - 8), Piece(board[m.to() - 8]), !SideToMove);   // same for black, just vice versa
      }
      playQuiet(Square(m.from()), Square(m.to()));
      break;

    case PR_KNIGHT:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Knight);
      break;
    case PR_BISHOP:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Bishop);
      break;
    case PR_ROOK:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Rook);
      break;
    case PR_QUEEN:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Queen);
      break;
    case PC_KNIGHT:
      history[halfMovesAfterStart].captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Knight);
      break;
    case PC_BISHOP:
      history[halfMovesAfterStart].captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Bishop);
      break;
    case PC_ROOK:
      history[halfMovesAfterStart].captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Rook);
      break;
    case PC_QUEEN:
      history[halfMovesAfterStart].captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Queen);
      break;
  }

  SideToMove = !SideToMove;
}

// plays a quiet move
void Board::playQuiet(Square from, Square to) {
  occB[SideToMove] ^= toBb(from) | toBb(to);
  Piece moved = Piece(board[from]);  // todo delete this
  board[from] = NoPiece;
  board[to] = moved;
  pieces[SideToMove][moved] ^= toBb(from) | toBb(to);

//  drawB(occB[SideToMove]);
//  cout<<(board[m.from()]) << " " << board[m.to()] <<"\n";
//  drawB(Pieces[SideToMove][moved]);
}

// undoes the last move m
void Board::undo(const Move &m) {
  SideToMove = !SideToMove;
  switch(m.flags()) {
    case DOUBLE_PUSH:
      epSqBb = 0ull;
    case QUIET:
      undoQuiet(Square(m.from()), Square(m.to()));
      break;

    case OO:
      undoQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == White)
        undoQuiet(h1, f1);
      else
        undoQuiet(h8, f8);
      break;

    case OOO:
      undoQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == White)
        undoQuiet(a1, d1);
      else
        undoQuiet(a8, d8);
      break;

    case CAPTURE:
      undoQuiet(Square(m.from()), Square(m.to()));
      addPiece(Square(m.to()), history[halfMovesAfterStart].captured, !SideToMove);
      break;

    case EN_PASSANT:
      undoQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == White)
        addPiece(Square(m.to() + 8), history[halfMovesAfterStart].captured);
      else
        addPiece(Square(m.to() - 8), history[halfMovesAfterStart].captured);
      break;

    case PR_KNIGHT:
    case PR_BISHOP:
    case PR_ROOK:
    case PR_QUEEN:
      addPiece(Square(m.from()), Pawn);
      removePiece(Square(m.to()), Pawn, SideToMove);
      break;

    case PC_KNIGHT:
    case PC_BISHOP:
    case PC_ROOK:
    case PC_QUEEN:
      addPiece(Square(m.from()), Pawn);
      removePiece(Square(m.to()), Pawn, SideToMove);
      addPiece(Square(m.to()), history[halfMovesAfterStart].captured);
      break;
  }
  --halfMovesAfterStart;
}


//undoes last quiet move m
void Board::undoQuiet(Square from, Square to) {
  playQuiet(to, from);
}

void Board::addPiece(Square sq, Piece piece) {
  addPiece(sq, piece, SideToMove);
}

void Board::addPiece(Square sq, Piece piece, Color color ) {
  occB[color] |= toBb(sq);
  board[sq] = piece;
  pieces[color][piece] |= toBb(sq);
}

////todo very weird arrangment
//void Board::removePiece(Square sq) {
//  removePiece(sq, Piece(board[sq]));
//}
//
////removes a piece
//void Board::removePiece(Square sq, Piece piece) {
//  removePiece(sq, piece, !SideToMove); // todo whyyyy
//}

// todo if returning the removed piece it could be used in undoQuiet (perfomance difference??)
void  Board::removePiece(Square sq, Piece piece, Color color) {
  occB[color] &= ~toBb(sq);
  board[sq] = NoPiece;
  pieces[color][piece] &= ~toBb(sq);
//  drawB(occB[color]);
//  drawB(Pieces[color][piece]);
}



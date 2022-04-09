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
  if (SideToMove == WHITE)
    att = 0x200000000000000, sq1 = e1, sq2 = c1, sq3 = g1;  //0x200000000000000 = b1    todo somehow make static
  else
    att = 2, sq1 = e8, sq2 = c8, sq3 = g8;  // 0x2 = b8

  //                                logical ! on integer means 0 => true else => false                    no piece can be on b1 or b8 for OOO
  bool QueenS = canCastle(QUEEN_SIDE, SideToMove) && !(getCastleBbs(QUEEN_SIDE, SideToMove) & attacked) && (att & ~getP());
  bool KingS = canCastle(KING_SIDE, SideToMove) && !(getCastleBbs(KING_SIDE, SideToMove) & attacked);

  //drawB(attacked);
  if (QueenS) *list++ = Move(sq1, sq2, OOO);
  if (KingS) *list++ = Move(sq1, sq3, OO);

  return list;
}

Move *Board::generatePawnM(Move *list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask,
                           uint64_t pos, int kingSq) {
  uint64_t promLine = SideToMove == WHITE ? getLines(a7, h7) : getLines(a2, h2);
  uint64_t epSqBb = history[halfMovesAfterStart].epSq;
  while (pos) {
    Square from = pop_lsb(pos);
    uint64_t doublePushMask = 0ull;
    uint64_t quiet = PawnMove(from, SideToMove, getP(), doublePushMask) & pushMask;
    uint64_t cap = PawnAttack(from, SideToMove) & captureMask;
    uint64_t pinnedLine = fullBb();

    if (toBb(from) & pinned) {
      quiet &= getLines(from, kingSq);
      cap &= getLines(from, kingSq);
      pinnedLine = getLines(from, kingSq);
      doublePushMask &= getLines(from, kingSq);
    }

    //quiet |= doublePushMask;
    if (quiet) {
      Square to = lsb(quiet);
      if (toBb(from) & promLine) {
        *list++ = Move(from, to, PR_KNIGHT);
        *list++ = Move(from, to, PR_BISHOP);
        *list++ = Move(from, to, PR_ROOK);
        *list++ = Move(from, to, PR_QUEEN);
      }
      else
        *list++ = Move(from, to, QUIET);
    }

    if(doublePushMask & pushMask) {
      Square to = lsb(doublePushMask);
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
      if (SideToMove == WHITE)
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
  uint64_t attacked = 0ull, temp;
  Square OpKing = lsb(getP(!SideToMove, King));
  Square OuKing = lsb(getP(SideToMove, King));

  uint64_t allPwoK = getP() & ~(1ull << OuKing); // bitboard with all Pieces except King of Playing side
  // Opponent Attacks
  temp = getP(!SideToMove, Pawn);
  while(temp) {
    Square from = pop_lsb(temp);
    attacked |= PawnAttack(from, !SideToMove);
  }

  temp = getP(!SideToMove, Knight);
  while(temp) {
    Square from = pop_lsb(temp);
    attacked |= KnightAttack(from);
  }

  temp = getP(!SideToMove, Rook);
  while(temp) {
    Square from = pop_lsb(temp);
    attacked |= mRook(from, allPwoK);
  }

  temp = getP(!SideToMove, Bishop);
  while(temp) {
    Square from = pop_lsb(temp);
    attacked |= mBishop(from, allPwoK);
  }

  temp = getP(!SideToMove, Queen);
  while(temp) {
    Square from = pop_lsb(temp);
    attacked |= mQueen(from, allPwoK);
  }

  attacked |= KingAttack(OpKing);
  attacked |= getP(SideToMove);  //Pieces of opponent are either capturable or attacked by other opp pieces

  list = generateKingM(list, attacked, getP(!SideToMove), OuKing);

  uint64_t checkers = attacksTo(OuKing,
                                !SideToMove); //maybe no need to check for queen and rook bc it can be done later with pinning step (s. surge-master)
  //drawB(checkers);

  uint64_t captureMask;
  uint64_t pushMask;

  int num_checkers = pop_cntll(checkers);

  if (num_checkers > 1)
    return list;

  else if (num_checkers == 1) {
    pushMask = getSquaresBetween(lsb(checkers), OuKing);
    captureMask = checkers;
  }

  else {
    captureMask = getP(!SideToMove);
    pushMask = ~getP();
    list = generateCastles(list, attacked | getP(!SideToMove));
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

// plays the move m on the board    stockfish somehow only with ifs and weird structure
void Board::play(const Move &m) {
  ++halfMovesAfterStart;

  history[halfMovesAfterStart] = UndoInfo(history[halfMovesAfterStart - 1]);
  UndoInfo &undoInfo = history[halfMovesAfterStart];

  if (board[m.from()] == Piece::Pawn || m.flags() & CAPTURE)
      undoInfo.halfMoves = 0;

  if (undoInfo.castlingRights && (getCastlingMask(m.to()) | getCastlingMask(m.from())));
    undoInfo.castlingRights &= ~(getCastlingMask(m.to()) | getCastlingMask(m.from()));

  switch(m.flags()) {
    case DOUBLE_PUSH:
      setEpSqFromDoublePush(Square(m.to()));
    case QUIET:
      playQuiet(Square(m.from()), Square(m.to()));
      break;

    case OO:
      playQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == WHITE) { // king moved alr, so rook has to move too
        undoInfo.castlingRights &= ~WHITE_OO;
        undoInfo.castlingRights &= ~WHITE_OOO;
        playQuiet(h1, f1);
      }
      else {
        undoInfo.castlingRights &= ~BLACK_OO;
        undoInfo.castlingRights &= ~BLACK_OOO;
        playQuiet(h8, f8);
      }
      break;

    case OOO:
      playQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == WHITE) { // king moved alr, so rook has to move too
        undoInfo.castlingRights &= ~WHITE_OO;
        undoInfo.castlingRights &= ~WHITE_OOO;
        playQuiet(a1, d1);
      }
      else {
        undoInfo.castlingRights &= ~BLACK_OO;
        undoInfo.castlingRights &= ~BLACK_OOO;
        playQuiet(a8, d8);
      }
      break;

    case CAPTURE:
      undoInfo.captured = Piece(board[m.to()]);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      playQuiet(Square(m.from()), Square(m.to()));
      break;

    case EN_PASSANT:
      if(SideToMove == WHITE) {
        undoInfo.captured = Piece(board[m.to() + 8]);
        removePiece(Square(m.to() + 8), Piece(board[m.to() + 8]), !SideToMove);   // the same as going one behind (e.g. e6 -> e5) from whites view
      }
      else {
        undoInfo.captured = Piece(board[m.to() - 8]);
        removePiece(Square(m.to() - 8), Piece(board[m.to() - 8]), !SideToMove);   // same for black, just vice versa
      }
      playQuiet(Square(m.from()), Square(m.to()));
      break;

    case PR_KNIGHT:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Knight, SideToMove);
      break;
    case PR_BISHOP:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Bishop, SideToMove);
      break;
    case PR_ROOK:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Rook, SideToMove);
      break;
    case PR_QUEEN:
      removePiece(Square(m.from()), Pawn, SideToMove);
      addPiece(Square(m.to()), Queen, SideToMove);
      break;

    case PC_KNIGHT:
      undoInfo.captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Knight, SideToMove);
      break;
    case PC_BISHOP:
      undoInfo.captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Bishop, SideToMove);
      break;
    case PC_ROOK:
      undoInfo.captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Rook, SideToMove);
      break;
    case PC_QUEEN:
      undoInfo.captured = Piece(board[m.to()]);
      removePiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), !SideToMove);
      addPiece(Square(m.to()), Queen, SideToMove);
      break;
  }

  SideToMove = !SideToMove;
}

// plays a quiet move
void Board::playQuiet(Square from, Square to) {
  occB[SideToMove] ^= toBb(from) | toBb(to);
  Piece moved = Piece(board[from]);  // save the piece that moved
  board[from] = NoPiece;
  board[to] = moved;
  pieces[SideToMove][moved] ^= toBb(from) | toBb(to);
}

// undoes the last move m
void Board::undo(const Move &m) {
  SideToMove = !SideToMove;
  switch(m.flags()) {
    case DOUBLE_PUSH:
    case QUIET:
      undoQuiet(Square(m.from()), Square(m.to()));
      break;

    case OO:
      undoQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == WHITE)
        undoQuiet(h1, f1);
      else
        undoQuiet(h8, f8);
      break;

    case OOO:
      undoQuiet(Square(m.from()), Square(m.to()));
      if(SideToMove == WHITE)
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
      if(SideToMove == WHITE)
        addPiece(Square(m.to() + 8), history[halfMovesAfterStart].captured, !SideToMove);
      else
        addPiece(Square(m.to() - 8), history[halfMovesAfterStart].captured, !SideToMove);
      break;

    case PR_KNIGHT:
    case PR_BISHOP:
    case PR_ROOK:
    case PR_QUEEN:
      addPiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), SideToMove);
      break;

    case PC_KNIGHT:
    case PC_BISHOP:
    case PC_ROOK:
    case PC_QUEEN:
      addPiece(Square(m.from()), Pawn, SideToMove);
      removePiece(Square(m.to()), Piece(board[m.to()]), SideToMove);
      addPiece(Square(m.to()), history[halfMovesAfterStart].captured, !SideToMove);
      break;
  }
  --halfMovesAfterStart;
}


//undoes last quiet move m
void Board::undoQuiet(Square from, Square to) {
  playQuiet(to, from);
}

void Board::addPiece(Square sq, Piece piece, Color color ) {
  occB[color] |= toBb(sq);
  board[sq] = piece;
  pieces[color][piece] |= toBb(sq);
}

// todo if returning the removed piece it could be used in undoQuiet (perfomance difference??) --wtf??
void  Board::removePiece(Square sq, Piece piece, Color color) {
  occB[color] &= ~toBb(sq);
  board[sq] = NoPiece;
  pieces[color][piece] &= ~toBb(sq);
//  drawB(occB[color]);
//  drawB(Pieces[color][piece]);
}




#include "move.h"
#include "attacks.h"
#include "utility.h"

#include <iostream>

using namespace std;


ostream &operator<<(ostream &os, Move &m) {
  os << printsq(m.from()) << printsq(m.to()) << " Type: " << m.flags() << "\n";
  return os;
}

ostream &operator<<(ostream &os, Move *m) {
  os << printsq(m->from()) << printsq(m->to()) << " Type: " << m->flags() << "\n";
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
  bool QueenS = castle[Queenside][SideToMove] && !(getCastleBbs(Queenside, SideToMove) & attacked) && (att & ~getP());
  bool KingS = castle[Kingside][SideToMove] && !(getCastleBbs(Kingside, SideToMove) & attacked);

  if (QueenS) *list++ = Move(sq1, sq2, OOO);
  if (KingS) *list++ = Move(sq1, sq3, OO);

  return list;
}

//todo double push -> e.p.
Move *Board::generatePawnM(Move *list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask,
                           uint64_t pos, int kingSq) {
  uint64_t promLine = SideToMove == White ? getLines(a7, h7) : getLines(a2, h2);
  while (pos) {
    Square from = pop_lsb(pos);
    uint64_t quiet = PawnMove(from, SideToMove, getP()) & pushMask;
    uint64_t cap = PawnAttack(from, SideToMove) & captureMask;
    uint64_t pinnedLine = fullBb();


    // drawB(promLine);
    // drawB(cap);

    if (toBb(from) & pinned) {
      quiet &= getLines(from, kingSq);
      cap &= getLines(from, kingSq);
      pinnedLine = getLines(from, kingSq);
    }

    while (quiet) {
      Square to = pop_lsb(quiet);
      if (toBb(from) & promLine) {
        *list++ = Move(from, to, PR_KNIGHT);
        *list++ = Move(from, to, PR_BISHOP);
        *list++ = Move(from, to, PR_ROOK);
        *list++ = Move(from, to, PR_QUEEN);
      } else
        *list++ = Move(from, to, QUIET);
    }

    while (cap) {
      Square to = pop_lsb(cap);
      if (toBb(from) & promLine) {
        *list++ = Move(from, to, PC_KNIGHT);
        *list++ = Move(from, to, PC_BISHOP);
        *list++ = Move(from, to, PC_ROOK);
        *list++ = Move(from, to, PC_QUEEN);
      } else
        *list++ = Move(from, to, CAPTURE);
    }

    // en passant
    if (PawnAttack(from, SideToMove) & epSqBb) {
      uint64_t epLine;   // determining where the square of captured pawn ist
      if (SideToMove == White)
        epLine = getLines(a5, h5);
      else
        epLine = getLines(a4, h4);

      uint64_t capSqBb = epLine & mRook(lsb(epSqBb), 0ull); //overlap of these two masks is this Square
      if (((epSqBb & pushMask & pinnedLine)    //if the pawn ends up on a moveable square
           || (capSqBb & captureMask & pinnedLine)))  // if pawn captures capturable square
      {
        //checking if the king is in check (from rook or queen) after ep on the horizontal axis
        // function: look if king is in check after taking away these two pawns by calc mRook from the king sq
        //   lookup piece on square by logical and with rook and queen bitboard of opponent
        //also checking diagonal line, after capturing a pawn
        uint64_t occwoPawn = getP() & ~(toBb(from) | capSqBb);
        uint64_t kingCheck = (epLine & mRook(kingSq, occwoPawn) & (getP(!SideToMove, Rook) | getP(!SideToMove, Queen)))
                             | (getLines(kingSq, lsb(capSqBb)) & mBishop(kingSq, occwoPawn) &
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
                                   Pawn);   //maybe remove the conversion to vector and doing it with while(bitboard) pop_lsb(bitboard) (s. surge-master)
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
  } else {
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

// plays the move m on the board TODO: implement updating history and switch case with moves
void Board::play(const Move &m) {
  SideToMove = !SideToMove;
  ++fullMoves;
  history[fullMoves] = UndoInfo(history[fullMoves - 1]);
  history[fullMoves].epSq = pop_lsb(epSqBb);

}



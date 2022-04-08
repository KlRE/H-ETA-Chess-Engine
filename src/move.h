#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <iostream>
#include "utility.h"
#include "board.h"
#include "attacks.h"


using namespace std;


ostream &operator<<(ostream &os, Move &m);

ostream &operator<<(ostream &os, Move *m);

// move class, consisting of where the piece moved from, where the piece will move to and the type of move
class Move {
private:
  uint16_t move;

public:
  Move() { move = 0; }

  Move(uint32_t from, uint32_t to) {
    assignValues(from, to);
  }

  Move(uint32_t from, uint32_t to, uint32_t flags) {
    assignValues(from, to, flags);
  }

  //for debugging
  Move(string move, const Board &board) {
    Square from = strToSq(move.substr(0,2));
    Square to = strToSq(move.substr(2,2));
    bool promoted = false;
    // check if the Pawn is being promoted
    uint64_t promLine = board.getSideToMove() == WHITE ? getLines(a7, h7) : getLines(a2, h2);

    if (board.getPieceOn(from) == Pawn && toBb(to) & promLine)
      promoted = true;

    if (board.getPieceOn(from) == King) {
      if ((from == e1 && to == g1) ||(from == e8 && to == g8))
        assignValues(from, to, OO);
      else if ((from == e1 && to == c1) || (from == e8 && to == c8))
        assignValues(from, to, OOO);
      else
        assignValues(from, to);
    }
    else if (board.getPieceOn(to) == NoPiece) {
      if (board.getPieceOn(from) == Pawn) {
        if (promoted) {
          switch (move[4]) {
            case 'q':
              assignValues (from, to, PR_QUEEN);
              break;
            case 'r':
              assignValues(from, to, PR_ROOK);
              break;
            case 'b':
              assignValues(from, to, PR_BISHOP);
              break;
            case 'n':
              assignValues(from, to, PR_KNIGHT);
              break;
          }
        }
        else if (toBb(from) & PawnAttack(from, board.getSideToMove()))
          assignValues(from, to, EN_PASSANT);
        else if(abs(from - to) > 9)
          assignValues(from, to, DOUBLE_PUSH);
        else
          assignValues(from, to);
      }
      else
        assignValues(from, to);
    }
    else {
      if(promoted) {
        switch (move[4]) {
          case 'q':
            assignValues (from, to, PC_QUEEN);
            break;
          case 'r':
            assignValues(from, to, PC_ROOK);
            break;
          case 'b':
            assignValues(from, to, PC_BISHOP);
            break;
          case 'n':
            assignValues(from, to, PC_KNIGHT);
            break;
        }
      }
      else
        assignValues(from, to, CAPTURE);

    }
  }

  Square to() const { return Square(move & 0x3f); }
  Square from() const { return Square((move >> 6) & 0x3f); }

  int to_from() const { return move & 0xffff; }

  MoveFlags flags() const { return MoveFlags((move >> 12) & 0xf); }

  void assignValues (uint32_t from, uint32_t to) {
    move = ((from & 0x3f) << 6) | (to & 0x3f);
  }

  void assignValues(uint32_t from, uint32_t to, uint32_t flags) {
    move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
  }
};


class MoveList {
private:
  Move list[218];
  Move *last;

public:
  MoveList();

  MoveList(Board &b) {
    last = b.generateMoves(list);
  }

  Move *begin() { return list; }

  Move *end() { return last; }

  size_t size() { return last - list; }

  void print() {
    Move *mv = begin();
    while (mv != end()) {
      cout << mv++;
    }
  }
};


#endif

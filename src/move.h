#ifndef MOVE_H
#define MOVE_H

#include <cstdint>
#include <iostream>
#include "utility.h"
#include "board.h"

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
    move = ((from & 0x3f) << 6) | (to & 0x3f);
  }

  Move(uint32_t from, uint32_t to, uint32_t flags) {
    move = ((flags & 0xf) << 12) | ((from & 0x3f) << 6) | (to & 0x3f);
  }


  uint32_t to() const { return move & 0x3f; }

  uint32_t from() const { return (move >> 6) & 0x3f; }

  int to_from() const { return move & 0xffff; }

  MoveFlags flags() const { return MoveFlags((move >> 12) & 0xf); }

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

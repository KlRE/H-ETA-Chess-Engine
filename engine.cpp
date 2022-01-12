#include <string>
#include <chrono>

#include "engine.h"
#include "board.h"
#include "move.h"
#include "attacks.h"

using namespace std;

uint64_t perftRec(int depth, Board &board) {
  MoveList ml(board);

  if(depth == 1) {
    return ml.size();
  }

  Move *cursor = ml.begin();
  uint64_t sum = 0;

  while(cursor != ml.end()) {
    board.play(*cursor);
    sum += perftRec(depth - 1, board);
    board.undo(*cursor);
    ++cursor;
  }
  if(sum==0) {
    cout<<"af";
  }
  return sum;
}

void perft(int depth, string fen) {
  initAttacks();

  auto start = chrono::system_clock::now();
  Board board(fen);

  uint64_t nodes =  perftRec(depth, board);
  printf("%d Nodes searched for depth %d\n", nodes, depth);

  chrono::duration<double,micro> tm = chrono::system_clock::now() - start;
  cout<<"Time taken by program: "<<tm.count()<<" microseconds\n";
  printf("Speed: %f Nodes/second", nodes / tm.count() * 1000000);
}
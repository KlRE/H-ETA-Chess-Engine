#include <string>
#include <chrono>

#include "engine.h"
#include "board.h"
#include "move.h"
#include "attacks.h"

using namespace std;

uint64_t perftRec(int depth, Board &board, uint64_t &nodes, uint64_t &captures, uint64_t &castles, uint64_t &promotions) {
  MoveList ml(board);

  if(depth == 1) {
    return ml.size();
  }

  Move *cursor = ml.begin();
  uint64_t sum = 0;

  while(cursor != ml.end()) {
    //for(int i=0; i<depth; i++) cout<<"    "; cout<<"-";  //prints moves
    //cout << *cursor <<"\n";
    if (cursor->flags() == CAPTURE)  ++captures;
    else if (cursor->flags() == OO || cursor->flags() == OOO )  ++captures;
    else if (cursor->flags() >= 0b1000)  ++promotions;  // for reference view Moveflags integers in utility.h

    board.play(*cursor);
    sum += perftRec(depth - 1, board, nodes, captures, castles, promotions);
    board.undo(*cursor);
    ++cursor;

  }
  return sum;
}

void perft(int depth, string fen) {
  initAttacks();

  auto start = chrono::system_clock::now();
  Board board(fen);
  uint64_t nodes = 0, nodes2;
  uint64_t captures = 0;
  uint64_t castles = 0;
  uint64_t promotions = 0;

  for (int i = 1; i <= depth; i++) {
    nodes2=perftRec(i, board, nodes, captures, castles, promotions);
    printf("%d Nodes searched for depth %d\nCaptures: %d\nCastles: %d\nPromotions: %d\n",
           nodes2, i, captures, castles, promotions);
    board.drawBoard();
  }

  chrono::duration<double,micro> tm = chrono::system_clock::now() - start;
  cout<<"Time taken by program: "<<tm.count()<<" microseconds\n";
  printf("Speed: %f Nodes/second\n\n ------------------------------------------------------------\n\n", nodes / tm.count() * 1000000);
}
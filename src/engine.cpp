#include <string>
#include <chrono>

#include "engine.h"
#include "board.h"
#include "move.h"
#include "attacks.h"

using namespace std;

void perftRec (int depth, Board &board, uint64_t &nodes, uint64_t &captures, uint64_t &castles, uint64_t &promotions, uint64_t &ep) {
  MoveList ml(board);
  Move *cursor = ml.begin();

  while(cursor != ml.end()) {

//    for(int i=0; i<depth; i++) cout<<"    "; cout<<"-";  //prints moves
//      cout << *cursor <<"\n";
//    if (__builtin_popcountll (board.getP(Black, King)) != 1)
//      board.drawBoard();


    if (depth == 0) {
      if (cursor->flags() & CAPTURE) ++captures;
      else if (cursor->flags() == OO || cursor->flags() == OOO) ++castles;
      else if (cursor->flags() >= 0b1000) ++promotions;  // for reference view Moveflags integers in utility.h
      else if(cursor->flags() == EN_PASSANT) ++ep;
      ++nodes;
    }

    board.play(*cursor);
     board.checkInternRep();  //todo remove if not debug
    if (depth != 0)
      perftRec(depth - 1, board, nodes, captures, castles, promotions, ep);
    board.undo(*cursor);
    ++cursor;
  }

}

// returns if the board internal representation is correct
bool perft(int depth, string fen) {
  initAttacks();

  auto start = chrono::system_clock::now();
  Board board(fen);
  uint64_t nodes, captures, castles, promotions, ep;

  for (int i = 0; i < depth; i++) {
    nodes = 0;
    captures = 0;
    castles = 0;
    promotions = 0;
    ep = 0;

    perftRec(i, board, nodes, captures, castles, promotions, ep);
    printf("%d Nodes searched for depth %d\nCaptures: %d\nCastles: %d\nPromotions: %d\nEn Passents: %d\n",
           nodes, i + 1, captures, castles, promotions, ep);
    //board.drawBoard();
  }

  chrono::duration<double,micro> tm = chrono::system_clock::now() - start;
  cout<<"Time taken by program: "<<tm.count()<<" microseconds\n";
  printf("Speed: %f Nodes/second\n\n ------------------------------------------------------------\n\n", nodes / tm.count() * 1000000);

  return board.checkInternRep();
}

void play ();
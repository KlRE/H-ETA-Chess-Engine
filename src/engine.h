#ifndef CHESSGIT_ENGINE_H
#define CHESSGIT_ENGINE_H

#include "board.h"

using namespace std;

class engine {

};

Move findBest(int depth, Board &board);
void iterativeDeepening(string fen, int depth);
void iterativeDeepening(Board &board, int maxDepth);
int alphaBeta(int depth, Board &board, int alpha, int beta);

void perftRec(int depth, Board &board, int &nodes, int &captures, int &castles, int &promotions, int &ep);
uint64_t perft(int, string, bool = true);
uint64_t perft(int depth, Board board, bool printAll = true);


#endif //CHESSGIT_ENGINE_H

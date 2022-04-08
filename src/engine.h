#ifndef CHESSGIT_ENGINE_H
#define CHESSGIT_ENGINE_H

#include "board.h"

using namespace std;

class engine {

};

void perftRec(int depth, Board &board, int &nodes, int &captures, int &castles, int &promotions, int &ep);
uint64_t perft(int, string, bool = true);
uint64_t perft(int depth, Board board, bool printAll = true);


#endif //CHESSGIT_ENGINE_H

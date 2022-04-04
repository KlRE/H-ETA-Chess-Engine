#ifndef CHESSGIT_ENGINE_H
#define CHESSGIT_ENGINE_H

#include "board.h"

using namespace std;

class engine {

};

void perftRec(int depth, Board &board, int &nodes, int &captures, int &castles, int &promotions, int &ep);
bool perft(int, string);


#endif //CHESSGIT_ENGINE_H

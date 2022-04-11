#ifndef CHESSGIT_UCI_H
#define CHESSGIT_UCI_H

#include "board.h"

void go(Board& pos, istringstream& is);
void position(Board& pos, istringstream& is);
void loop(int argc, char* argv[]);

#endif

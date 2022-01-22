#include <iostream>
#include <vector>
#include <chrono>					   // only for taking record of time taken by program

#include "test.h"

using namespace std;

/* TODO:
 *  apparently puts pieces back with wrong color
 *  bug checkers somehow for fen depth 6 black side always 2, so perft(6) wrong
 *  game continues after mate
 *  last: obstructed für pinned pieces (s. firefox) (idk if solved??))
   - move function
    - add ep square after ep see test5

   - unit tests for:
      - play
      - generateMoves, bishop, pawn etc..
    - finish halfmove clock


   - adding const and Reference in Parameter of functions (e.g. in attacks.h/.cpp)
*/



int main()
{
  alltests();
}


#include <iostream>
#include <vector>
#include <chrono>					   // only for taking record of time taken by program

#include "test.h"

using namespace std;

/* TODO:
   - last: obstructed für pinned pieces (s. firefox) (idk if solved??))
   - move function
    - suddenly no castling rights?
    - add ep square after ep see test5

   - unit tests for:
      - play
      - generateMoves, bishop, pawn etc..
    - finish halfmove clock


   - adding const and Reference in Parameter of functions (e.g. in attacks.h/.cpp)
*/



int main()
{
  auto start = chrono::system_clock::now();
  alltests();
  
  chrono::duration<double,milli> tm = chrono::system_clock::now() - start;
  cout<<"Time taken by program: "<<tm.count()<<" ms"<<endl;
}


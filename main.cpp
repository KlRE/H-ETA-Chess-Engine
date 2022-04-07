#include "src/test.h"
#include "board.h"
#include "move.h"

using namespace std;

/* TODO:
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



int main(int argc, char* argv[])
{
  alltests();
  int depth = stoi(string(argv[1]));
  string fen = string(argv[2]);
  Board board(fen);
  if (argc > 3) {
    argc -= 3;
    while(--argc) {
      string move = string(argv[argc]);
      board.play(Move(move));
    }
  }
  MoveList ml(board);
  ml.print();
}


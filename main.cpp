#include <iostream>
#include <string>

#include "src/test.h"
#include "utility.h"
#include "board.h"
#include "attacks.h"
#include "move.h"
#include "engine.h"


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



int main(int argc, char *argv[]) {
  //alltests();
  initAttacks();

  int depth = stoi(string(argv[1]));
  string fen = argv[2];
  //cout << depth;
  Board board((string)fen);
  for(int i = 3; i < argc; i++) {
    Move move((string)argv[i], board);
    board.play(Move((string)argv[i], board));
  }

  MoveList ml(board);
  Move *cursor = ml.begin();
  string output[ml.size()];

  for(int i = 0; i < ml.size(); ++cursor, ++i) {
    board.play(*cursor);
    output[i] = printSq(cursor->from()) + printSq(cursor->to()) + " " + to_string(perft(depth - 1, board, false));
    board.undo(*cursor);
  }

  sort(output, output + ml.size());
  for (string s : output) cout << s << "\n";

  cout << "\n" << perft(depth, board, false) << "\n";

}


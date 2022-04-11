#include <iostream>
#include <sstream>
#include <cassert>

#include "uci.h"
#include "attacks.h"
#include "move.h"
#include "engine.h"


/// inspired from stockfish: uci.cpp

// FEN string of the initial position, normal chess
const char* StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// go() is called when engine receives the "go" UCI command. The function sets
// the thinking time and other parameters from the input string, then starts
// the search.
// position startpos moves e2e4 a7a6 d2d4 b7b6 f2f4 e7e6 c2c4 c7c6 b2b4 f8b4 c1d2 a6a5 d2b4 a5b4 d1b3 d7d6 g1f3 f7f5 e4f5 e6f5 f1d3 e8f7 e1g1 c6c5 f3g5 f7g6 d4c5 h7h6 g5h3 b6c5 b3c2 g6h5 b1d2 h5h4 d3f5 c8b7 d2f3 b7f3 f1f3 h4h5 f3g3 h5h4 f5g6 a8a3 c2f5 h6h5 a1f1 a3g3 h2g3 h4g3 f1e1 g3h4 e1e3
// position startpos moves e2e4 a7a6 d1h5 b7b6 f1c4 e7e6 g1f3 e8e7 f3e5 g7g6 h5f3 f7f6 e5g4 e7f7 f3f4 f7g7 e4e5 f6f5 g4e3 g7f7 g2g4 f7g7 g4g5 g7f7 e3f5 g6f5 f4f5 f7g7 h2h4 e6f5 h4h5 d7d5 h5h6 g7g6 c4d3 d8g5 b1c3 g5g2 h1f1 g6h5 c3d5 g2d5 f1g1 h5h6 d3e2 g8e7 d2d4
void go(Board& pos, istringstream& is) {
//  string token;
//
//  limits.startTime = now(); // As early as possible!
//
//  while (is >> token)
//    if (token == "searchmoves") // Needs to be the last command on the line
//      while (is >> token)
//        limits.searchmoves.push_back(UCI::to_move(pos, token));
//
//    else if (token == "wtime")     is >> limits.time[WHITE];
//    else if (token == "btime")     is >> limits.time[BLACK];
//    else if (token == "winc")      is >> limits.inc[WHITE];
//    else if (token == "binc")      is >> limits.inc[BLACK];
//    else if (token == "movestogo") is >> limits.movestogo;
//    else if (token == "depth")     is >> limits.depth;
//    else if (token == "nodes")     is >> limits.nodes;
//    else if (token == "movetime")  is >> limits.movetime;
//    else if (token == "mate")      is >> limits.mate;
//    else if (token == "perft")     is >> limits.perft;
//    else if (token == "infinite")  limits.infinite = 1;
//    else if (token == "ponder")    ponderMode = true;
//
//  Threads.start_thinking(pos, states, limits, ponderMode);


  iterativeDeepening(pos, 4);

}

// position() is called when engine receives the "position" UCI command.
// The function sets up the position described in the given FEN string ("fen")
// or the starting position ("startpos") and then makes the moves given in the
// following move list ("moves").
void position(Board& pos, istringstream& is) {
  string token, fen;
  is >> token;

  if (token == "startpos") {
    fen = StartFEN;
    is >> token; // Consume "moves" token if any
  }
  else if (token == "fen")
    while (is >> token && token != "moves")
      fen += token + " ";
  else
    return;

  pos.setFen(fen);

  // Parse move list (if any)
  while (is >> token) {
    pos.play(Move(token, pos));
//    Move m(token, pos);
//    cout << m <<"\n";
//    pos.drawBoard();
    assert(pos.checkInternRep());
  }

}
/// UCI::loop() waits for a command from stdin, parses it and calls the appropriate
/// function. Also intercepts EOF from stdin to ensure gracefully exiting if the
/// GUI dies unexpectedly. When called with some command line arguments, e.g. to
/// run 'bench', once the command is executed the function returns immediately.
/// In addition to the UCI ones, also some additional debug commands are supported.
void loop(int argc, char* argv[]) {
  Board pos;
  string token, cmd;

  for (int i = 1; i < argc; ++i)
    cmd += std::string(argv[i]) + " ";

  do {
    if (argc == 1 && !getline(cin, cmd)) // Block here waiting for input or EOF
      cmd = "quit";

    istringstream is(cmd);

    token.clear(); // Avoid a stale if getline() returns empty or blank line
    is >> skipws >> token;

    if (token == "uci")
      cout << "id name H:ETA"// << engine_info(true)
                 << "\nid author erikw"
                //<< "\n"       << Options
                << "\nuciok"  << endl;

//    else if (token == "setoption")  setoption(is);
    else if (token == "go")         go(pos, is);
    else if (token == "position")   position(pos, is);
    else if (token == "ucinewgame") pos.clearBoard();
    else if (token == "isready")    initAttacks(), cout << "readyok" << endl;

      // Additional custom non-UCI commands, mainly for debugging.
      // Do not use these commands during a search!
    else if (token == "d")        pos.drawBoard();
    /*else if (token == "flip")     pos.flip();
    else if (token == "bench")    bench(pos, is, states);
    else if (token == "eval")     trace_eval(pos);
    else if (token == "compiler") sync_cout << compiler_info() << sync_endl; */

    else if (!token.empty() && token[0] != '#')
      cout << "Unknown command: " << cmd << endl;

  } while (token != "quit" && argc == 1); // Command line args are one-shot
}

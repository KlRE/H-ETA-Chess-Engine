#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <climits>

using namespace std;

static const int INF = INT_MAX;

class Move;

enum Square : int{
  a8, b8, c8, d8, e8, f8, g8, h8,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a1, b1, c1, d1, e1, f1, g1, h1,
  NoSquare
};

enum Piece : int
{
  NoPiece, Pawn, Rook, Knight, Bishop, Queen, King
};

enum Color : bool { WHITE, BLACK };  //parameter as bool, bc ! operator leads to casting

enum MoveFlags : int {
  QUIET = 0b0000, DOUBLE_PUSH = 0b0001,
  OO = 0b0010, OOO = 0b0011,
  CAPTURE = 0b0100,
  EN_PASSANT = 0b0101,
  PR_KNIGHT = 0b1000, PR_BISHOP = 0b1001, PR_ROOK = 0b1010, PR_QUEEN = 0b1011,
  PC_KNIGHT = 0b1100, PC_BISHOP = 0b1101, PC_ROOK = 0b1110, PC_QUEEN = 0b1111,
};


// stockfish types.h
enum CastlingRights {
  NO_CASTLING,
  WHITE_OO,
  WHITE_OOO = WHITE_OO << 1,
  BLACK_OO  = WHITE_OO << 2,
  BLACK_OOO = WHITE_OO << 3,

  KING_SIDE      = WHITE_OO  | BLACK_OO,
  QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
  WHITE_CASTLING = WHITE_OO  | WHITE_OOO,
  BLACK_CASTLING = BLACK_OO  | BLACK_OOO,
  ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

  CASTLING_RIGHT_NB = 16
};


// todo maybe instead of ternary operator with constant bitshifts
constexpr CastlingRights operator&(Color c, CastlingRights cr) {
  return CastlingRights((c == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & cr);
}
CastlingRights getCastlingMask(int square);
uint64_t getCastleBbs(CastlingRights castle, Color color);

void initCastleBbs();

inline int pop_cnt(uint64_t bb) { return __builtin_popcount(bb);}
inline int pop_cntll(uint64_t bb) { return __builtin_popcountll(bb); }
inline Square lsb(uint64_t i) { return Square(__builtin_ffsll(i)-1); }
Square pop_lsb(uint64_t &i);
Square strToSq(string s);

inline uint64_t toBb(int i) { return 1ull<<i; }

inline uint64_t fullBb() { return 0xFFFFFFFFFFFFFFFF; }

uint64_t makeMask(vector<int> v);

//vector<int> getPos(uint64_t i); // returns positions of all bits, which are on on bitboard
vector<Square> getPos(uint64_t i); // returns positions of all bits, which are on on bitboard

ostream &operator<<(ostream &os, Square sq);
string printSq(int i);

ostream &operator<<(ostream &os, Piece p);
char printShortPiece(Piece p, Color color);

ostream &operator<<(ostream &os, Color color);
Color operator!(Color color);

ostream &operator<<(ostream &os, MoveFlags mv);
string printMty(int i);

void dividePerftOutput();
#endif

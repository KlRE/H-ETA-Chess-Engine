#include <cstdint>
#include <vector>
#include <iostream>

#include "utility.h"


using namespace std;

// returns the smallest bit of a bitboard as an square
Square pop_lsb(uint64_t &i)
{
  int out = __builtin_ffsll(i);
  i^=1ull<<(out-1);  // schneller: i &= i-1
  return Square(out-1);
}

Square strToSq(string s)
{
  int col = tolower(s[0]) - 'a';
  int row = 8 - (tolower(s[1]) - '0');
  return Square(row*8 + col);
}

uint64_t makeMask(vector<int> v)
{
  uint64_t out=0ull;
  for(int i:v)
    out |= 1ull<<i;
  return out;
}

// vector<int> getPos(uint64_t i) // returns positions of all bits, which are on on bitboard
// {
//   vector<int> ret;
//   while(i != 0)
//     {
//       //cout<<(63 - __builtin_clzll(i))<<"\n";
//       ret.push_back(63 - __builtin_clzll(i));
//       i -= 1ull<<(63 - __builtin_clzll(i));
//     }
//   return ret;
// }

vector<Square> getPos(uint64_t i) // returns positions of all bits, which are on on bitboard
{
  vector<Square> ret;
  while(i != 0)
    {
      //cout<<(63 - __builtin_clzll(i))<<"\n";
      ret.push_back( static_cast<Square> (63 - __builtin_clzll(i)));
      i -= 1ull<<(63 - __builtin_clzll(i));
    }
  return ret;
}
string ChessSq[] = {
  "a8","b8", "c8", "d8", "e8", "f8", "g8", "h8",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

ostream &operator<<(ostream &os, Square sq) {  return os<<ChessSq[sq]; }

string printsq(int i) 		// prints enum or int of a square as string 
{
  return ChessSq[i];
}

string pieceName[] = {   "NoPiece", "Pawn", "Rook", "Knight", "Bishop", "Queen", "King"};
ostream &operator<<(ostream &os, Piece p) { return os<<pieceName[p]; }
string shortPiece[] = { "", "", "r", "n", "b", "q", "k"};
string printShortPiece(Piece p) { return shortPiece[p]; }

string colors[] = { "White", "Black" };
ostream &operator<<(ostream &os, Color color)  { return os << colors[color]; }
Color operator!(Color color)
{
  if(color == White) return Black;
  else return White;
}

string Mtype[] =  {
  "QUIET", "DOUBLE_PUSH",
  "OO", "OOO",
  "CAPTURE",
  "EN_PASSANT" ,"","",
  "PR_KNIGHT", "PR_BISHOP", "PR_ROOK" , "PR_QUEEN",
  "PC_KNIGHT", "PC_BISHOP", "PC_ROOK", "PC_QUEEN"
};

ostream &operator<<(ostream &os, MoveFlags mv) {return os << Mtype[mv];}
  
string printMty(int i) {  return Mtype[i]; }


int popcount(uint64_t i) { return __builtin_popcountll(i); }

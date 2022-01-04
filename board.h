#ifndef BOARD_H
#define BOARD_H

#include <vector> 
#include <cstdint>
#include <string>

#include "utility.h"

using namespace std;


//Stores position information which cannot be recovered on undo-ing a move
struct UndoInfo {
  //number of halfmoves for 50-move-rule
  int halfmoves;
	
  //The piece that was captured on the last move
  Piece captured;
	
  //The en passant square. This is the square which pawns can move to in order to en passant capture an enemy pawn that has 
  //double pushed on the previous move
  Square epsq;

  constexpr UndoInfo() : halfmoves(0), captured(NoPiece), epsq(NoSquare) {}
	
  //This preserves the entry bitboard across moves
  UndoInfo(const UndoInfo& prev) : 
    halfmoves(prev.halfmoves + 1), captured(NoPiece), epsq(NoSquare) {}
};


class Board
{
private:
  uint64_t occB[2] = {};
  uint64_t Pieces[2][7] = {};
  int board[64] = {};
  bool castle[2][2] = {};

  Color SideToMove = White;
  uint64_t epSqBb = 0ull;
  int halfMovesC = 0;
  int fullMoves = 0;

public:

  Board();
  explicit Board(string s);
  explicit Board(char Arr[64], Color color, string castling, Square epSq) { setArr(Arr, color, castling, epSq); }

  uint64_t getP() { return occB[0] | occB[1]; }   // returns bitboard with all pieces
  uint64_t getP(Color color){ return occB[color]; }  // returns bitboard with pieces of one color
  uint64_t getP(Color color, Piece piece) { return Pieces[color][piece]; }  // returns bitboard of piece of one color
  Piece pieceOn(int sq) { return Piece(board[sq]); }
  
  vector<Square> PieceSqs(Color color, Piece piece) { return getPos(Pieces[color][piece]); }
  
  Color side() {return SideToMove; };

  uint64_t attacksTo(Square sq, Color color); //returns bitboard with squares with pieces attacking square sq
  uint64_t sliderBlockers(Square sq, Color color);

  void clearBoard();
  
  void setPos(int, Piece, Color);
  void setFen(string fen);
  void setArr(char Arr[64], Color color, string castling, Square epSq);
  void setColor(Color color) { SideToMove = color; }
  void setEpSq(int sq) { epSqBb = 1ull<<sq; }
  
  void drawBoard();

  /* *****************************************
     ************** in move.cpp **********
     ***************************************** */
  
  Move *generateKingM(Move *list, const uint64_t &attacked, const uint64_t &OpPieces, Square sq);
  Move* generateCastles(Move *list, const uint64_t &attacked);
  Move* generatePawnM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
  Move* generateRookM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
  Move* generateKnightM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos);
    Move* generateBishopM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
    Move* generateQueenM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
  
  Move *generateMoves(Move *list);

  void play(const Move &m);
};

void drawB(uint64_t bb, bool showNum = false);

#endif

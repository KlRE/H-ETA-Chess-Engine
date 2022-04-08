#ifndef BOARD_H
#define BOARD_H

#include <vector> 
#include <cstdint>
#include <string>

#include "utility.h"

using namespace std;

void drawB(uint64_t bb, bool showNum = false);  //TODO: move to utility
void drawSquareChar(bool black);

// todo: make history array like stockfish position.h line 197
//Stores position information which cannot be recovered on undo-ing a move
struct UndoInfo {
  //number of halfmoves for 50-move-rule
  int halfMoves;
	
  //The piece that was captured on the last move
  Piece captured;
	
  //The en passant square. This is the square which pawns can move to in order to en passant capture an enemy pawn that has 
  //double pushed on the previous move
  uint64_t epSq;

  // castle rights for both sides
  int castlingRights;

  UndoInfo() : halfMoves(0), captured(NoPiece), epSq(0), castlingRights(NO_CASTLING) {}
	
  //This preserves the entry bitboard across moves
  //edited: no bitboard is
  UndoInfo(const UndoInfo& prev) :
          halfMoves(prev.halfMoves + 1), captured(NoPiece), epSq(0), castlingRights(prev.castlingRights){}
};


class Board
{
private:
  static const int MAX_HISTORY = 256;

  uint64_t occB[2] = {};       // occurenceBoard for b/w, used as blockermask
  uint64_t pieces[2][7] = {};  // piecelist for black and white, used to find all pieces of one color
  int board[64] = {};          // mailbox representation, used to finding piece on specific square
  //bool castle[2][2] = {};      // castling rights castle[Side][Color]
  UndoInfo history[MAX_HISTORY] = {};  // unrecoverable information like ep square

  Color SideToMove; // side to move
  //uint64_t epSqBb; // en passant square bitboard
  int startingFullMoves; // half moves since start of game
  int halfMovesAfterStart; // plys after starting full moves

public:
  Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {};
  Board(string s);
  Board(char Arr[64], Color color, string castling, Square epSq) { setArr(Arr, color, castling, epSq); }

  uint64_t getP() const { return occB[0] | occB[1]; }   // returns bitboard with all pieces
  uint64_t getP(Color color) const { return occB[color]; }  // returns bitboard with pieces of one color
  uint64_t getP(Color color, Piece piece) const { return pieces[color][piece]; }  // returns bitboard of piece of one color
  Piece getPieceOn(int sq) const { return (Piece)board[sq]; }  // returns piece on square
  Color getSideToMove() const { return SideToMove; }

  bool canCastle(CastlingRights cr, Color color) const {
    return color &  CastlingRights(history[halfMovesAfterStart].castlingRights & cr);
  }

  vector<Square> PieceSqs(Color color, Piece piece) { return getPos(pieces[color][piece]); }
  
  Color side() {return SideToMove; };

  int getFullMoves() { return startingFullMoves + (halfMovesAfterStart + abs(SideToMove-1)) / 2; }

  uint64_t attacksTo(Square sq, Color color); //returns bitboard with squares with pieces attacking square sq
  uint64_t sliderBlockers(Square sq, Color color);

  void clearBoard();
  void resetHistory(); //todo implement

  void setPos(int, Piece, Color);
  void setFen(string fen);
  string toFen();
  void setArr(char Arr[64], Color color, string castling, Square epSq);
  void setColor(Color color) { SideToMove = color; }
  void setEpSqFromDoublePush(int sq) { history[halfMovesAfterStart].epSq = 1ull << (sq + (SideToMove == Color::WHITE ? 8 : -8)); }
  
  void drawBoard();

  // only for debugging
  // returns if the internal representation of this board is consistent
  bool checkInternRep();

  /* *****************************************
     ************** in move.cpp **********
     ***************************************** */
  
  Move* generateKingM(Move *list, const uint64_t &attacked, const uint64_t &OpPieces, Square sq);
  Move* generateCastles(Move *list, const uint64_t &attacked);
  Move* generatePawnM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
  Move* generateRookM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
  Move* generateKnightM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos);
  Move* generateBishopM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
  Move* generateQueenM(Move* list, const uint64_t &pinned, const uint64_t &pushMask, const uint64_t &captureMask, uint64_t pos, int kingSq);
  
  Move *generateMoves(Move *list);


  void play(const Move &m);                          // plays the move m
  void playQuiet(Square from, Square to);           // plays the quiet move m
  void undo(const Move &m);                          // undoes move m
  void undoQuiet(Square from, Square to);            // undoes quiet move m


  // adds a piece on the square with a specific color
  void addPiece(Square sq, Piece piece, Color color);


  // removes the piece on Square sq of any color (should be the right one)
  void removePiece(Square sq, Piece piece, Color color);
};



#endif

#ifndef BOARD_H
#define BOARD_H

#include <vector> 
#include <cstdint>
#include <string>

#include "utility.h"

using namespace std;

void drawB(uint64_t bb, bool showNum = false);  //TODO: move to utility

//Stores position information which cannot be recovered on undo-ing a move
struct UndoInfo {
  //number of halfmoves for 50-move-rule
  int halfmoves;
	
  //The piece that was captured on the last move
  Piece captured;
	
  //The en passant square. This is the square which pawns can move to in order to en passant capture an enemy pawn that has 
  //double pushed on the previous move
  uint64_t epSq;

  // castle rights for both sides  TODO: implement castle in UndoInfo
  bool castle[2][2];

  UndoInfo() : halfmoves(0), captured(NoPiece), epSq(NoSquare) {}
	
  //This preserves the entry bitboard across moves
  //edited: no bitboard is
  UndoInfo(const UndoInfo& prev) :
          halfmoves(prev.halfmoves + 1), captured(NoPiece), epSq(NoSquare){}
};


class Board
{
private:
  uint64_t occB[2] = {};       // occurenceBoard for b/w, used as blockermask
  uint64_t Pieces[2][7] = {};  // piecelist for black and white, used to find all pieces of one color TODO: change to lowercase
  int board[64] = {};          // mailbox representation, used to finding piece on specific square
  bool castle[2][2] = {};      // castling rights
  UndoInfo history[256] = {};  // unrecoverable information like ep square

  Color SideToMove;
  uint64_t epSqBb;
  int fullMoves;

public:
  Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {};
  Board(string s);
  Board(char Arr[64], Color color, string castling, Square epSq) { setArr(Arr, color, castling, epSq); }

  uint64_t getP() { return occB[0] | occB[1]; }   // returns bitboard with all pieces
  uint64_t getP(Color color){ return occB[color]; }  // returns bitboard with pieces of one color
  uint64_t getP(Color color, Piece piece) { return Pieces[color][piece]; }  // returns bitboard of piece of one color
//  Piece pieceOn(int sq) { return Piece(board[sq]); }
//  void setPiece(int sq, Piece piece) { board[sq] = piece; }
  
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

  // adds the Piece piece to Square sq with the color of the player, whose turn it is
  void addPiece(Square sq, Piece piece);
  // adds a piece on the square with a specific color
  void addPiece(Square sq, Piece piece, Color color);

  // removes the piece on Square sq of the color of the player, whose turn it is (should be the right one)
  void removePiece(Square sq);
  // removes the Piece piece on Square sq of the color of the player, whose turn it is (should be the right one)
  void removePiece(Square sq, Piece piece);
  // removes the piece on Square sq of any color (should be the right one)
  void removePiece(Square sq, Piece piece, Color color);


};



#endif

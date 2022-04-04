#include <iostream>
#include <iomanip>
#include <cstdint>

#include "board.h"
#include "utility.h"
#include "attacks.h"

using namespace std;


//TODO: migrate epSqBb to history (replace all instances of ep with h[0] or maybe not for better readability
Board::Board(string s) {
  clearBoard();
  setFen(s);
}


void Board::setPos(int pos, Piece piece, Color color) {
  pieces[color][piece] |= 1ull << pos;
  occB[color] |= 1ull << pos;
  board[pos] = piece;
}

bool Board::checkInternRep() {
  // checking if all piece bitboards and the color bitboard are aquivalent
  // checking for duplicates in piece bitboards for each color and piece
  // "direct sum" of all pieces bitboards must be equal to the color bitboard
  for (int i = 0; i < 2; i++) {  // for each color
    uint64_t allPieceBb = 0ull;
    for (int j = 1; j < 7; j++) { // for each piece (first piece is noPiece)
      for (int k = j + 1; k < 7; k++) { // with each piece
        if (pieces[i][j] & pieces[i][k]) {
          cout << "duplicate piece in color " << Color(i) << " and Pieces: " << Piece(j) << " and " << Piece(k) << ".\n";
          return false;
        }
      }
      allPieceBb |= pieces[i][j];
    }
    if (allPieceBb != occB[i]) {
      drawB(allPieceBb);
      drawB(occB[i]);
      cout << "Piece bitboards don't match the occurrence board of Color "  << Color(i) << ".\n";
      return false;
    }
  }

  // checking if every piece from the all piece board is also on the piece-color bitboard
  for (int i = 0; i < 2; i++) {
    uint64_t occBb = occB[i];
    while (occBb) {
      Square sq = pop_lsb(occBb);
      Piece piece = Piece(board[sq]);
      if ((toBb(sq) & pieces[i][piece]) == 0) { // if the square is not occupied by the piece
        cout << "Square " <<  printSq(sq) << " in the Occurence Board from Color " << Color(i) << "is not occupied in the "
        << Piece(piece) << " bitboard\n";
        return false;
      }
    }
  }
  return true;
}


uint64_t Board::attacksTo(Square sq, Color color) {
  uint64_t occ = getP();
  return (PawnAttack(sq, !color) & getP(color, Pawn)) //pawn other color because reverse from square
         | (KnightAttack(sq) & getP(color, Knight))
         | (mRook(sq, occ) & (getP(color, Rook) | getP(color, Queen)))
         | (mBishop(sq, occ) & (getP(color, Bishop) | getP(color, Queen)))
         | (KingAttack(sq) & getP(color, King));
}

// returns bitboard with pieces of own color that block opponent sliders from attacking Square sq
// maybe include parameter with pieces to exclude, bc check pieces count too
uint64_t Board::sliderBlockers(Square sq, Color color)  // inspiration from stockfish:position.cpp
{
  uint64_t blockers = 0ull;
  uint64_t snipers = ((getP(!color, Rook) | getP(!color, Queen)) & mRook(sq, 0ull)) |
                     ((getP(!color, Bishop) | getP(!color, Queen)) & mBishop(sq, 0ull));
  //drawB(snipers);

  while (snipers) {
    Square s = pop_lsb(snipers);
    uint64_t blocks = getSquaresBetween(s, sq) & getP();
    if (popcount(blocks) == 1 && blocks & getP(color)) {
      blockers |= blocks;
    }
  }

  return blockers;
}

/* ****************************************
*************  Miscellaneous  **********
**************************************** */

void Board::clearBoard() {  // todo maybe replace this in constructor
  fill(occB, occB + 2, 0);
  fill(&pieces[0][0], &pieces[0][0] + sizeof(pieces) / sizeof(pieces[0][0]), 0);
  fill(board, board + 64, 0);
  fill(&castle[0][0], &castle[0][0] + sizeof(castle) / sizeof(castle[0][0]), 0);
  fill(history, history + MAX_HISTORY, UndoInfo());

  SideToMove = White;
  epSqBb = 0ull;
  halfMovesAfterStart = 0;
  startingFullMoves = 1;
}


void Board::setFen(string fen) {
  clearBoard();

  int idx = 0;
  for (int pos = 0; pos < 64; pos++) {
    if (idx == (int) fen.size())break;
    char c = fen[idx++];
    switch (c) {
      case 'p':
        setPos(pos, Pawn, Black);
        break;
      case 'r':
        setPos(pos, Rook, Black);
        break;
      case 'n':
        setPos(pos, Knight, Black);
        break;
      case 'b':
        setPos(pos, Bishop, Black);
        break;
      case 'q':
        setPos(pos, Queen, Black);
        break;
      case 'k':
        setPos(pos, King, Black);
        break;
      case 'P':
        setPos(pos, Pawn, White);
        break;
      case 'R':
        setPos(pos, Rook, White);
        break;
      case 'N':
        setPos(pos, Knight, White);
        break;
      case 'B':
        setPos(pos, Bishop, White);
        break;
      case 'Q':
        setPos(pos, Queen, White);
        break;
      case 'K':
        setPos(pos, King, White);
        break;
      case '/':    // since pos is incremented everytime
        --pos;
        break;
      case '1':
        break;
      case '2':
        ++pos;
        break;
      case '3':
        pos += 2;
        break;
      case '4':
        pos += 3;
        break;
      case '5':
        pos += 4;
        break;
      case '6':
        pos += 5;
        break;
      case '7':
        pos += 6;
        break;
      case '8':
        pos += 7;
        break;

        //default: return -1;
    }
  }
  idx++;

  if (fen[idx] == 'b')  //cursor in Black/White position
    SideToMove = Black;
  else
    SideToMove = White;

  idx += 2;

  while (fen[idx] != ' ') // cursor in castle position
  {
    char c = fen[idx++];
    switch (c) {
      case 'K':
        castle[Kingside][White] = true;
        break;
      case 'Q':
        castle[Queenside][White] = true;
        break;
      case 'k':
        castle[Kingside][Black] = true;
        break;
      case 'q':
        castle[Queenside][Black] = true;
        break;
    }
  }
  // printf("%d %d %d %d\n", castle[1][0],castle[0][0],castle[1][1], castle[0][1]);
  idx++;
  //  drawB(toBb(strToSq(string() + fen[idx++] + fen[idx])));
  if (fen[idx] != '-') //cursor in e.p. position
    epSqBb = toBb(strToSq(string() + fen[idx] + fen[idx + 1])), idx++;

  if (fen.size() - idx > 2) {
    idx += 2;
    int idx2 = 0;
    while (fen[idx + idx2] != ' ')   //cursor in halfmoves clock position
      idx2++;

    history[0].halfMoves = stoi(fen.substr(idx, idx + idx2));
    idx += idx2 + 1;
    startingFullMoves = stoi(fen.substr(idx, fen.size() - idx)); //what is left in string is fullmoves
  }
}

// returns a string describing the current position in FEN notation
string Board::toFen() {
  string fen = "";

  // piece placement
  int empty = 0;
  for (int i = 0; i  < 64; i++) {
    Piece piece = Piece(board[i]);
    Color color = (pieces[Black][piece] & toBb(i)) ? Color::Black : Color::White;
    if (piece == NoPiece) {
      empty++;
    } else {
      if (empty > 0) {
        fen += to_string(empty);
        empty = 0;
      }
      fen += printShortPiece(piece, color);
    }
    //add a slash between ranks
    if ((i + 1) % 8 == 0) {
      if (empty > 0) {
        fen += to_string(empty);
        empty = 0;
      }
      if (i != 63) {
        fen += "/";
      }
    }
  }

  // add turn
  fen += string(" ") + (SideToMove == Color::White ? "w " : "b ");

  // add when no castling rights
  if (!castle[Kingside][White] && !castle[Queenside][White] && !castle[Kingside][Black] && !castle[Queenside][Black]) {
    fen += "- ";
  } else {
    if (castle[Kingside][White]) {
      fen += "K";
    }

    if (castle[Queenside][White]) {
      fen += "Q";
    }
    if (castle[Kingside][Black]) {
      fen += "k";
    }
    if (castle[Queenside][Black]) {
      fen += "q";
    }
    fen += " ";
  }

  // add en passant square
  if (epSqBb != 0ull) {
    fen += printSq(lsb(epSqBb)) + " ";
  } else {
    fen += "- ";
  }

  //add halfmove clock
  fen += to_string(history[halfMovesAfterStart].halfMoves) + " ";

  // add full moves
  fen += to_string(getFullMoves());

  return fen;
}


// TODO: add halfmoves clock param
void Board::setArr(char Arr[64], Color color, string castling,
                   Square epSq) // when calling this function add color as parameter, otherwise calls wrong one
{
  clearBoard();
  SideToMove = color;

  for (int pos = 0; pos < 64; pos++) {
    char c = Arr[pos];
    switch (c) {
      case 'p':
        setPos(pos, Pawn, Black);
        break;
      case 'r':
        setPos(pos, Rook, Black);
        break;
      case 'n':
        setPos(pos, Knight, Black);
        break;
      case 'b':
        setPos(pos, Bishop, Black);
        break;
      case 'q':
        setPos(pos, Queen, Black);
        break;
      case 'k':
        setPos(pos, King, Black);
        break;
      case 'P':
        setPos(pos, Pawn, White);
        break;
      case 'R':
        setPos(pos, Rook, White);
        break;
      case 'N':
        setPos(pos, Knight, White);
        break;
      case 'B':
        setPos(pos, Bishop, White);
        break;
      case 'Q':
        setPos(pos, Queen, White);
        break;
      case 'K':
        setPos(pos, King, White);
        break;
    }
  }
  int idx = 0;
  while (idx < castling.size() && castling[idx] != '-') {
    char c = castling[idx++];
    switch (c) {
      case 'K':
        castle[Kingside][White] = true;
        break;
      case 'Q':
        castle[Queenside][White] = true;
        break;
      case 'k':
        castle[Kingside][Black] = true;
        break;
      case 'q':
        castle[Queenside][Black] = true;
        break;
    }
  }
  //  printf("%d %d %d %d\n", castle[1][0],castle[0][0],castle[1][1], castle[0][1]);

  epSqBb = toBb(epSq);
}

void Board::drawBoard() {
  string BArr[8][8]{};
  vector<Square> pos;

  pos = PieceSqs(Black, Pawn);
  for (int i: pos) BArr[i / 8][i % 8] = "pa";
  pos = PieceSqs(Black, Rook);
  for (int i: pos) BArr[i / 8][i % 8] = "ro";
  pos = PieceSqs(Black, Knight);
  for (int i: pos) BArr[i / 8][i % 8] = "kn";
  pos = PieceSqs(Black, Bishop);
  for (int i: pos) BArr[i / 8][i % 8] = "bi";
  pos = PieceSqs(Black, Queen);
  for (int i: pos) BArr[i / 8][i % 8] = "qu";
  pos = PieceSqs(Black, King);
  for (int i: pos) BArr[i / 8][i % 8] = "ki";

  pos = PieceSqs(White, Pawn);
  for (int i: pos) BArr[i / 8][i % 8] = "PA";
  pos = PieceSqs(White, Rook);
  for (int i: pos) BArr[i / 8][i % 8] = "RO";
  pos = PieceSqs(White, Knight);
  for (int i: pos) BArr[i / 8][i % 8] = "KN";
  pos = PieceSqs(White, Bishop);
  for (int i: pos) BArr[i / 8][i % 8] = "BI";
  pos = PieceSqs(White, Queen);
  for (int i: pos) BArr[i / 8][i % 8] = "QU";
  pos = PieceSqs(White, King);
  for (int i: pos) BArr[i / 8][i % 8] = "KI";

  for (int y = 0; y < 3 * 8; y++) {
    for (int x = 0; x < 6 * 8; x++) {
      if (y % 3 == 1 && x % 6 == 2 && BArr[y / 3][x / 6] != "")
        cout << BArr[y / 3][x / 6], x++;
      else
        //cout << static_cast<char>((x % 12 < 6 && y % 6 < 3) || ((x % 12 > 5 && y % 6 > 2)) ? 219 : 177);
        drawSquareChar((x % 12 < 6 && y % 6 < 3) || ((x % 12 > 5 && y % 6 > 2)));
    }
    cout << "\n";
  }
  cout << "\n";

  cout << "Castling Rights: " << (castle[Queenside][SideToMove] ? "Queenside " : "")
       << (castle[Kingside][SideToMove] ? "Kingside" : "") << "\n";

  if (epSqBb)
    cout << "En Passent Square: " << Square(lsb(epSqBb)) << "\n";
  else
    cout << "No En Passent Square \n";

  cout << SideToMove << " to Move: \n";

}

void drawB(uint64_t bb, bool showNum) {
  for (int y = 0; y < 3 * 8; y++) {
    for (int x = 0; x < 6 * 8; x++) {
      if (y % 3 == 1 && x % 6 == 2 && bb & 1ull << (y / 3 * 8 + x / 6)) {
        if (showNum)
          cout << setw(2) << y / 3 * 8 + x / 6;
        else
          cout << "  ";
        x++;
      } else
        drawSquareChar((x % 12 < 6 && y % 6 < 3) || ((x % 12 > 5 && y % 6 > 2)));


    }
    cout << "\n";
  }
  cout << "\n";
}

void drawSquareChar(bool black) {
 if (black)
   cout << "█";
   //cout << static_cast<char> (219);
 else
   cout << "▒";
   //cout << static_cast<char> (177);
}



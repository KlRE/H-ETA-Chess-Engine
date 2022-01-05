#include <iostream>
#include <iomanip>
#include <cstdint>

#include "board.h"
#include "utility.h"
#include "attacks.h"

using namespace std;


//TODO: migrate epSqBb to history (replace all instances of ep with h[0] or maybe not for better readability
Board::Board(string s) {
  epSqBb = 0ull;
  fullMoves = 0;
  history[0] = UndoInfo();
  setFen(s);
}

void Board::setPos(int pos, Piece piece, Color color) {
  Pieces[color][piece] |= 1ull << pos;
  occB[color] |= 1ull << pos;
  board[pos] = piece;
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

void Board::clearBoard() {
  fill(occB, occB + 2, 0);
  fill(&Pieces[0][0], &Pieces[0][0] + sizeof(Pieces) / sizeof(Pieces[0][0]), 0);
  fill(board, board + 64, 0);

  SideToMove = White;
  epSqBb = 0ull;
}


void Board::setFen(string fen) {
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

    history[0].halfmoves = stoi(fen.substr(idx, idx + idx2));
    idx += idx2 + 1;
    fullMoves = stoi(fen.substr(idx, fen.size() - idx)); //what is left in string is fullmoves
  }
}

// TODO: add halfmoves clock param
void Board::setArr(char Arr[64], Color color, string castling,
                   Square epSq) // when calling this function add color as parameter, otherwise calls wrong one
{
  SideToMove = color;
  epSqBb = 0ull;
  fullMoves = 0;
  history[0] = UndoInfo();

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
        cout << static_cast<char>((x % 12 < 6 && y % 6 < 3) || ((x % 12 > 5 && y % 6 > 2)) ? 219 : 177);
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
        cout << static_cast<char>((x % 12 < 6 && y % 6 < 3) || ((x % 12 > 5 && y % 6 > 2)) ? 219 : 177);
    }
    cout << "\n";
  }
  cout << "\n";
}



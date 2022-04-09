#include "board.h"
#include "types.h"

int Board::evaluate() const {
  int score = 0;
  // Evaluate material depending on piece type
  for (int i = 1; i < 6; i++) {
    score +=  PIECE_VALUES[i] * (pop_cntll(getP(SideToMove, Piece(i))) - pop_cntll(getP(!SideToMove, Piece(i))));
  }
  return score;

}

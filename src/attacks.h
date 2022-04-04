#ifndef ATTACKS_H
#define ATTACKS_H

#include<utility.h>

using namespace std;



void initAttacks();

void initSquaresBetween();
uint64_t getSquaresBetween(int sq1, int sq2);

void initLines();
uint64_t getLines(int sq1, int sq2);

void initCastleBbs();
uint64_t getCastleBbs(Castles castle, Color color);

/* ********************************
   **********  Bishop  ************
   ******************************** */


uint64_t BishopAttack(int sq, uint64_t occ, bool attmask);

uint64_t mBishop(int sq, uint64_t occ);
    
void initBishop(bool test = false);


/* ***************************************
   ****************  Rook  ***************
   ***************************************  */

uint64_t RookAttack(int sq, uint64_t occ, bool attmask);

uint64_t mRook(int sq, uint64_t occ);
  
void initRook(bool test = false);

/* ***************************************************
   ********************  Queen  **********************
   *************************************************** */
  
uint64_t mQueen(int sq, uint64_t occ);


/* **************************************************
   ****************** Knight ************************
   ************************************************** */

uint64_t KnightAttack(int sq);	// generates Knight attacks from square sq

void initKnight();
  
/* *********************************************
   ********************  King  *****************
   ********************************************* */
  
uint64_t KingAttack(int sq); 	// generate KingAttacks from Square

void initKing();

/* *****************************************
   *****************  Pawn  ****************
   ***************************************** */

uint64_t PawnAttack(int sq, bool color);
uint64_t PawnMove(int sq, bool color, const uint64_t &occ, uint64_t &doublePushMask);

#endif

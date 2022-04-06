#include <iostream>
#include <cstdint>

#include "board.h"
#include "attacks.h"

using namespace std;

uint64_t magicBishopAtt[64][512];
const uint64_t magicBishop[64] = {
  0x0002020202020200ull, 0x0002020202020000ull, 0x0004010202000000ull,
  0x0004040080000000ull, 0x0001104000000000ull, 0x0000821040000000ull,
  0x0000410410400000ull, 0x0000104104104000ull, 0x0000040404040400ull,
  0x0000020202020200ull, 0x0000040102020000ull, 0x0000040400800000ull,
  0x0000011040000000ull, 0x0000008210400000ull, 0x0000004104104000ull,
  0x0000002082082000ull, 0x0004000808080800ull, 0x0002000404040400ull,
  0x0001000202020200ull, 0x0000800802004000ull, 0x0000800400A00000ull,
  0x0000200100884000ull, 0x0000400082082000ull, 0x0000200041041000ull,
  0x0002080010101000ull, 0x0001040008080800ull, 0x0000208004010400ull,
  0x0000404004010200ull, 0x0000840000802000ull, 0x0000404002011000ull,
  0x0000808001041000ull, 0x0000404000820800ull, 0x0001041000202000ull,
  0x0000820800101000ull, 0x0000104400080800ull, 0x0000020080080080ull,
  0x0000404040040100ull, 0x0000808100020100ull, 0x0001010100020800ull,
  0x0000808080010400ull, 0x0000820820004000ull, 0x0000410410002000ull,
  0x0000082088001000ull, 0x0000002011000800ull, 0x0000080100400400ull,
  0x0001010101000200ull, 0x0002020202000400ull, 0x0001010101000200ull,
  0x0000410410400000ull, 0x0000208208200000ull, 0x0000002084100000ull,
  0x0000000020880000ull, 0x0000001002020000ull, 0x0000040408020000ull,
  0x0004040404040000ull, 0x0002020202020000ull, 0x0000104104104000ull,
  0x0000002082082000ull, 0x0000000020841000ull, 0x0000000000208800ull,
  0x0000000010020200ull, 0x0000000404080200ull, 0x0000040404040400ull,
  0x0002020202020200ull
};

const uint64_t magicBmask[64] = {
  0x0040201008040200ull, 0x0000402010080400ull, 0x0000004020100A00ull,
  0x0000000040221400ull, 0x0000000002442800ull, 0x0000000204085000ull,
  0x0000020408102000ull, 0x0002040810204000ull, 0x0020100804020000ull,
  0x0040201008040000ull, 0x00004020100A0000ull, 0x0000004022140000ull,
  0x0000000244280000ull, 0x0000020408500000ull, 0x0002040810200000ull,
  0x0004081020400000ull, 0x0010080402000200ull, 0x0020100804000400ull,
  0x004020100A000A00ull, 0x0000402214001400ull, 0x0000024428002800ull,
  0x0002040850005000ull, 0x0004081020002000ull, 0x0008102040004000ull,
  0x0008040200020400ull, 0x0010080400040800ull, 0x0020100A000A1000ull,
  0x0040221400142200ull, 0x0002442800284400ull, 0x0004085000500800ull,
  0x0008102000201000ull, 0x0010204000402000ull, 0x0004020002040800ull,
  0x0008040004081000ull, 0x00100A000A102000ull, 0x0022140014224000ull,
  0x0044280028440200ull, 0x0008500050080400ull, 0x0010200020100800ull,
  0x0020400040201000ull, 0x0002000204081000ull, 0x0004000408102000ull,
  0x000A000A10204000ull, 0x0014001422400000ull, 0x0028002844020000ull,
  0x0050005008040200ull, 0x0020002010080400ull, 0x0040004020100800ull,
  0x0000020408102000ull, 0x0000040810204000ull, 0x00000A1020400000ull,
  0x0000142240000000ull, 0x0000284402000000ull, 0x0000500804020000ull,
  0x0000201008040200ull, 0x0000402010080400ull, 0x0002040810204000ull,
  0x0004081020400000ull, 0x000A102040000000ull, 0x0014224000000000ull,
  0x0028440200000000ull, 0x0050080402000000ull, 0x0020100804020000ull,
  0x0040201008040200ull
};

uint64_t magicRookAtt[64][4096];
const uint64_t magicRook[64] = {
  0x0080001020400080ull, 0x0040001000200040ull, 0x0080081000200080ull,
  0x0080040800100080ull, 0x0080020400080080ull, 0x0080010200040080ull,
  0x0080008001000200ull, 0x0080002040800100ull, 0x0000800020400080ull,
  0x0000400020005000ull, 0x0000801000200080ull, 0x0000800800100080ull,
  0x0000800400080080ull, 0x0000800200040080ull, 0x0000800100020080ull,
  0x0000800040800100ull, 0x0000208000400080ull, 0x0000404000201000ull,
  0x0000808010002000ull, 0x0000808008001000ull, 0x0000808004000800ull,
  0x0000808002000400ull, 0x0000010100020004ull, 0x0000020000408104ull,
  0x0000208080004000ull, 0x0000200040005000ull, 0x0000100080200080ull,
  0x0000080080100080ull, 0x0000040080080080ull, 0x0000020080040080ull,
  0x0000010080800200ull, 0x0000800080004100ull, 0x0000204000800080ull,
  0x0000200040401000ull, 0x0000100080802000ull, 0x0000080080801000ull,
  0x0000040080800800ull, 0x0000020080800400ull, 0x0000020001010004ull,
  0x0000800040800100ull, 0x0000204000808000ull, 0x0000200040008080ull,
  0x0000100020008080ull, 0x0000080010008080ull, 0x0000040008008080ull,
  0x0000020004008080ull, 0x0000010002008080ull, 0x0000004081020004ull,
  0x0000204000800080ull, 0x0000200040008080ull, 0x0000100020008080ull,
  0x0000080010008080ull, 0x0000040008008080ull, 0x0000020004008080ull,
  0x0000800100020080ull, 0x0000800041000080ull, 0x00FFFCDDFCED714Aull,
  0x007FFCDDFCED714Aull, 0x003FFFCDFFD88096ull, 0x0000040810002101ull,
  0x0001000204080011ull, 0x0001000204000801ull, 0x0001000082000401ull,
  0x0001FFFAABFAD1A2ull
};
const uint64_t magicRmask[64] = {
  0x000101010101017Eull, 0x000202020202027Cull, 0x000404040404047Aull,
  0x0008080808080876ull, 0x001010101010106Eull, 0x002020202020205Eull,
  0x004040404040403Eull, 0x008080808080807Eull, 0x0001010101017E00ull,
  0x0002020202027C00ull, 0x0004040404047A00ull, 0x0008080808087600ull,
  0x0010101010106E00ull, 0x0020202020205E00ull, 0x0040404040403E00ull,
  0x0080808080807E00ull, 0x00010101017E0100ull, 0x00020202027C0200ull,
  0x00040404047A0400ull, 0x0008080808760800ull, 0x00101010106E1000ull,
  0x00202020205E2000ull, 0x00404040403E4000ull, 0x00808080807E8000ull,
  0x000101017E010100ull, 0x000202027C020200ull, 0x000404047A040400ull,
  0x0008080876080800ull, 0x001010106E101000ull, 0x002020205E202000ull,
  0x004040403E404000ull, 0x008080807E808000ull, 0x0001017E01010100ull,
  0x0002027C02020200ull, 0x0004047A04040400ull, 0x0008087608080800ull,
  0x0010106E10101000ull, 0x0020205E20202000ull, 0x0040403E40404000ull,
  0x0080807E80808000ull, 0x00017E0101010100ull, 0x00027C0202020200ull,
  0x00047A0404040400ull, 0x0008760808080800ull, 0x00106E1010101000ull,
  0x00205E2020202000ull, 0x00403E4040404000ull, 0x00807E8080808000ull,
  0x007E010101010100ull, 0x007C020202020200ull, 0x007A040404040400ull,
  0x0076080808080800ull, 0x006E101010101000ull, 0x005E202020202000ull,
  0x003E404040404000ull, 0x007E808080808000ull, 0x7E01010101010100ull,
  0x7C02020202020200ull, 0x7A04040404040400ull, 0x7608080808080800ull,
  0x6E10101010101000ull, 0x5E20202020202000ull, 0x3E40404040404000ull,
  0x7E80808080808000ull
};

const unsigned magicRshift[64] = {
  52, 53, 53, 53, 53, 53, 53, 52,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 54, 54, 54, 54, 53,
  53, 54, 54, 53, 53, 53, 53, 53
};


uint64_t Nmask[64], Kmask[64];

uint64_t SquaresBetween[64][64];
uint64_t Lines[64][64];


void initAttacks()
{
  initBishop();
  initRook();
  initKnight();
  initKing();
  initSquaresBetween();
  initLines();
  initCastleBbs();
}

//todo move to utility
void initSquaresBetween()
{
  for(int i=0; i<64;i++)
    {
      for(int j=0; j<64; j++)
	{
	  uint64_t occ = 1ull<<i & 1ull<<j;

	  if(i%8 == j%8 || i/8 == j/8)
	    SquaresBetween[i][j] = mRook(i, 1ull<<j) & mRook(j, 1ull<<i);
	  else if(abs(i/8-j/8) == abs(i%8-j%8))
	    SquaresBetween[i][j] = mBishop(i, 1ull<<j) & mBishop(j, 1ull <<i);
	  else
	    SquaresBetween[i][j] = 0;
	}
    }
}

uint64_t getSquaresBetween(int sq1,int sq2) {return SquaresBetween[sq1][sq2]; }


void initLines()
{
    for(int i=0; i<64;i++)
    {
      for(int j=0; j<64; j++)
	{
	  uint64_t occ = 1ull<<i & 1ull<<j;

	  if(i%8 == j%8 || i/8 == j/8)
	    Lines[i][j] = (mRook(i, 0ull) & mRook(j, 0ull)) | 1ull<<i | 1ull<<j;
	  else if(abs(i/8-j/8) == abs(i%8-j%8))
	    Lines[i][j] = (mBishop(i, 0ull) & mBishop(j, 0ull)) | 1ull<<i | 1ull<<j;
	  else
	    Lines[i][j] = 0;
	}
    }
}

uint64_t getLines(int sq1, int sq2) { return Lines[sq1][sq2]; }
  

/* ********************************
**********  Bishop  ************
******************************** */


uint64_t BishopAttack(int sq, uint64_t occ, bool attmask)
{
  int r = sq % 8;
  int f = sq / 8;
    
  uint64_t mask = 0ull;

  for(int tr=r+1, tf=f+1; tr<7+attmask && tf<7+attmask; tr++, tf++)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }
  for(int tr=r+1, tf=f-1; tr<7+attmask && tf>0-attmask; tr++, tf--)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }
	
  for(int tr=r-1, tf=f-1; tr>0-attmask && tf>0-attmask; tr--, tf--)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }
  for(int tr=r-1, tf=f+1; tr>0-attmask && tf<7+attmask; tr--, tf++)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }  
    
  return mask;
}

uint64_t mBishop(int sq, uint64_t occ)
{
  // occ &= ~(1ull<<sq);		// necessary???  ->> no!
  occ &= magicBmask[sq];
  occ *= magicBishop[sq];
  occ >>= 64-9;

  return magicBishopAtt[sq][occ];
}
    
void initBishop(bool test)
{
  int found = 0;
  for(int i=0;i<64;i++)	// basically, creates all possible blockerboards for each sqaure
    {
      uint64_t mask = magicBmask[i], temp;	   // create blockerboard mask (with all possible occurences)
      int sqs = __builtin_popcountll(mask);		   // count blockerboard set bits
      vector<uint64_t> bits(sqs);			   // array with set bits of bb

      temp = mask;
      for(int j=0;j<sqs;j++)	                           // create array with all set bits of blockerboard
	{
	  bits[j] = 1ull<<(63 - __builtin_clzll(temp));
	  temp &= ~(1ull<<(63 - __builtin_clzll(temp)));	    
	}
	
      for(int j=0;j<(1<<sqs);j++)			   // loop through all subsets of blockerboard
	{
	  uint64_t bbmask = mask;			   // create one blockermask subset
	  int t = j;
	  for(int k=0; k<__builtin_popcount(j);k++)
	    {
	      int abit = 31 - __builtin_clz(t);
	      t-= 1<<(31 - __builtin_clz(t));
	      bbmask &=  ~bits[abit];
	    }
	    
	  uint64_t attmask = BishopAttack(i, bbmask, 1);   // create attackmask to that blockermask
	  // drawBoard(bbmask);
	  // drawBoard(attmask);
	  // cout<<"\n";
	    
	  if(!test)
	    {
	      bbmask *= magicBishop[i];
	      bbmask >>= 64-9;
	      magicBishopAtt[i][bbmask]= attmask;
	    }
	  else
	    {
	      uint64_t attmask2 = mBishop(i,bbmask);
	      if(attmask!=attmask2)
		cout<<attmask<<" "<<attmask2<<"\n",found++;		  
	    }
	}
    }
  if(test && !found) cout<<"Bishop magic numbers and initialization correct!!\n";
}


/* ***************************************
****************  Rook  ***************
***************************************  */

uint64_t RookAttack(int sq, uint64_t occ, bool attmask)
{
  int r = sq % 8;
  int f = sq / 8;
    
  uint64_t mask = 0ull;

  for(int tr=r+1, tf=f; tr<7+attmask; tr++)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }
  for(int tr=r, tf=f-1; tf>0-attmask; tf--)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }	
  for(int tr=r-1, tf=f; tr>0-attmask; tr--)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }
  for(int tr=r, tf=f+1; tf<7+attmask; tf++)
    {
      mask |= 1ull<<(8*tf+tr);
      if(occ & 1ull<<(8*tf+tr))
	break;
    }  
    
  return mask;
}

uint64_t mRook(int sq, uint64_t occ)
{
  // occ &= ~(1ull<<sq);		// necessary?? ->> no!
  // occ |= (1ull<<sq);	
  occ &= magicRmask[sq];
  occ *= magicRook[sq];
  occ >>= 64-12;//magicRshift[sq];

  return magicRookAtt[sq][occ];
}
  
void initRook(bool test)
{
  int found = 0;
  for(int i=0;i<64;i++)
    {
      uint64_t mask = magicRmask[i], temp;	   // create blockerboard mask (with all possible occurences)
      int sqs = __builtin_popcountll(mask);		   // count blockerboard set bits
      vector<uint64_t> bits(sqs);			   // array with set bits of bb

      temp = mask;
      for(int j=0;j<sqs;j++)	                           // create array with all set bits of blockerboard
	{
	  bits[j] = 1ull<<(63 - __builtin_clzll(temp));
	  temp &= ~(1ull<<(63 - __builtin_clzll(temp)));	    
	}
	
      for(int j=0;j<(1<<sqs);j++)			   // loop through all subsets of blockerboard
	{
	  uint64_t bbmask = mask;			   // create one blockermask subset
	  int t = j;
	  for(int k=0; k<__builtin_popcount(j);k++)
	    {
	      int abit = 31 - __builtin_clz(t);
	      t-= 1<<(31 - __builtin_clz(t));
	      bbmask &=  ~bits[abit];
	    }
	    
	  uint64_t attmask = RookAttack(i, bbmask, 1);   // create attackmask to that blockermask
	    
	  if(!test)
	    {
	      bbmask *= magicRook[i];
	      bbmask >>= 64-12;//magicRshift[i];
	      magicRookAtt[i][bbmask] = attmask;
	    }
	  else
	    {
	      uint64_t attmask2 = mRook(i, bbmask);
	      if(attmask!=attmask2)
		{
		  drawB(bbmask);
		  drawB(attmask);
		  drawB(attmask2);
		    
		  cout<<attmask<<" "<<attmask2<<"\n",found++;
		}
	    }
	}
    }
  if(test && !found) cout<<"Rook magic numbers and initialization correct!!\n";
}

/* ***************************************************
********************  Queen  **********************
*************************************************** */
  
uint64_t mQueen(int sq, uint64_t occ)
{
  return mBishop(sq,occ) | mRook(sq,occ);
}


/* **************************************************
****************** Knight ************************
************************************************** */

uint64_t KnightAttack(int sq)	// generates Knight attacks from square sq
{
  int r = sq % 8;
  int f = sq / 8;

  uint64_t mask = 0ull;

  if(r+2 < 8 && f+1 < 8)
    mask |= 1ull<<(8*(f+1)+r+2);
  if(r+1 < 8 && f+2 < 8)
    mask |= 1ull<<(8*(f+2)+r+1);
  if(r+2 < 8 && f-1 >= 0)
    mask |= 1ull<<(8*(f-1)+r+2);
  if(r+1 < 8 && f-2 >= 0)
    mask |= 1ull<<(8*(f-2)+r+1);
  if(r-2 >=0 && f+1 < 8)
    mask |= 1ull<<(8*(f+1)+r-2);
  if(r-1 >= 0 && f+2 < 8)
    mask |= 1ull<<(8*(f+2)+r-1);
  if(r-2 >= 0 && f-1 >= 0)
    mask |= 1ull<<(8*(f-1)+r-2);
  if(r-1 >= 0 && f-2 >= 0)
    mask |= 1ull<<(8*(f-2)+r-1);
    
    
  return mask;
}

void initKnight()
{
  for(int i=0;i<64;i++)
    Nmask[i] = KnightAttack(i);
}

/* *********************************************
********************  King  *****************
********************************************* */
  
uint64_t KingAttack(int sq) 	// generate KingAttacks from Square
{
  int r = sq % 8;
  int f = sq / 8;

  uint64_t mask = 0ull;
  
  if(r+1 < 8)
    mask |= 1ull<<(8*f + r+1);
  if(r-1 >= 0)
    mask |= 1ull<<(8*f + r-1);
  if(f+1 < 8)
    mask |= 1ull<<(8*(f+1) + r);
  if(f-1 >= 0)
    mask |= 1ull<<(8*(f-1) + r);
  if(r+1 < 8 && f+1 < 8)
    mask |= 1ull<<(8*(f+1) + r+1);
  if(r-1 >= 0 && f+1 < 8)
    mask |= 1ull<<(8*(f+1) + r-1);
  if(r+1 < 8 && f-1 >= 0)
    mask |= 1ull<<(8*(f-1) + r+1);
  if(r-1 >= 0 && f-1 >= 0)
    mask |= 1ull<<(8*(f-1) + r-1);
       
    
  return mask;
}

void initKing()
{
  for(int i=0;i<64;i++)
    Kmask[i] = KingAttack(i); 
}

/* *****************************************
*****************  Pawn  ****************
***************************************** */

uint64_t PawnAttack(int sq, bool color)
{
  int r = sq % 8;
  int f = sq / 8;

  uint64_t mask = 0ull;

  if(color == WHITE)
    {
      if(r > 0) mask |= 1ull << (8*(f-1) + r-1);
      if(r < 7) mask |= 1ull << (8*(f-1) + r+1);
    }
  else
    {
      if(r > 0) mask |= 1ull << (8*(f+1) + r-1);
      if(r < 7) mask |= 1ull << (8*(f+1) + r+1);      
    }
			 
  return mask;
}

// returns mask with single pushes and writes double pushes in argument
uint64_t PawnMove(int sq, bool color, const uint64_t &occ, uint64_t &doublePushMask)
{
  uint64_t mask = 0ull;
  int f = sq / 8;
  
  if(color == WHITE)
    {
      mask |= (1ull << (sq-8) & ~occ);
      if(f == 6 && mask)    //if mask is empty, there is piece blocking the pawn from moving 
	      doublePushMask |= (1ull<<(sq-16) & ~occ);
    }
  else
    {
      mask |= (1ull << (sq+8) & ~occ);
      if(f == 1 && mask)    //if mask is empty, there is piece blocking the pawn from moving 
	      doublePushMask |= (1ull<<(sq+16) & ~occ);
    }

  return mask;
}

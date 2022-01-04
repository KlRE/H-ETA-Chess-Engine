 #include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

#include "utility.h"
#include "attacks.h"
#include "board.h"
#include "move.h"
#include "test.h"

using namespace std;

/* TODO:
   - testing ep. function

 
*/

void alltests()
{
  // test1();
  test2();
  // test3();
  // test4();
}

string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0";
string perft1 = "";
string perft2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
string perft3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
string perft4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
string perft5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
string perft6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
  
string tst = "8/2p5/3p4/1P1K3r/1R3p1k/8/4P1P1/8 w - - ";
string tst1 = "8/8/8/8/1R3p1k/8/4P1P1/8 w Kk - ";

//               A    B    C    D    E    F    G    H
char bd[64] = { ' ', ' ', ' ', 'n', ' ', ' ', ' ', ' ', // 8
    
                ' ', ' ', 'P', ' ', ' ', ' ', ' ', ' ', // 7
 		
                ' ', ' ', ' ', 'p', ' ', ' ', ' ', ' ', // 6
		
                ' ', 'P', ' ', 'K', ' ', 'P', ' ', 'r', // 5
		
                ' ', ' ', 'n', ' ', ' ', ' ', 'P', 'k', // 4
		  
                'P', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 3
		
                ' ', ' ', ' ', ' ', 'P', ' ', 'P', ' ', // 2
		
                'q', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };//1
//               A    B    C    D    E    F    G    H
  
char bb[64] = { ' ', ' ', ' ', ' ', 'K', ' ', 'n', ' ', // 8
     
                ' ', ' ', ' ', ' ', ' ', 'P', ' ', ' ', // 7
 		
                ' ', ' ', ' ', ' ', 'k', ' ', 'b', ' ', // 6
		
                ' ', ' ', 'p', 'P', ' ', ' ', ' ', ' ', // 5
		
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 4
		  
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 3
		
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 2
		
                'R', ' ', ' ', ' ', ' ', ' ', ' ', 'R' };//1
//               A    B    C    D    E    F    G    H
  
char t3[64] = { 'r', ' ', ' ', ' ', ' ', ' ', ' ', 'r', // 8
    
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 7
 		
                ' ', ' ', ' ', ' ', ' ', 'k', ' ', ' ', // 6
		
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 5
		
                ' ', ' ', 'P', 'p', ' ', ' ', ' ', ' ', // 4
		  
                ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 3
		
                ' ', 'p', ' ', ' ', ' ', ' ', ' ', ' ', // 2
		
                'B', ' ', 'N', 'K', ' ', ' ', ' ', ' ' };// 1
//               A    B    C    D    E    F    G    H


// TODO: e.p. square after double push

void test()
{
  initAttacks();
  
  Board CBoard (tst);
  CBoard.drawBoard();
  
  uint64_t mask = 0ull;
  mask = makeMask(vector<int>{e4,e5,d4,d5,a3,b2,c6,d6,e1,f6,g8,h3});
  mask = makeMask(vector<int> {h5} );
  //CBoard.drawBoard();
  //MoveList ML(CBoard);

  // Board d(bd);
  // d.drawBoard();
  // MoveList MM(d);
  // MM.print();

  Board c(bb,White,"KQkq",c6);
  c.drawBoard();
  MoveList M(c);
  M.print();

  Board bb3(t3,Black,"kq",c3);
  bb3.drawBoard();
  MoveList M3(bb3);
  M3.print();

  // Board def;
  // MoveList ML2(def);
  // ML2.print();

  
  // // CBoard.initRook(1);
  // for(int i=0;i<64;i++){ CBoard.drawBoard(mask);
  //   CBoard.drawBoard(CBoard.mQueen(i,mask));cout<<"\n\n\n\n";}

  // CBoard.test(mask);
  
  
  //cin>>fen;
  // drawB(mask);
  // for(int i=8;i<56;i++) drawB(PawnMove(i,Black,mask));
}


void test2()
{
  initAttacks();
  Board bb1(fen), bb2(perft2), bb3(perft3), bb4(perft4), bb5(perft5), bb6(perft6);
  bb1.drawBoard();
  MoveList M1(bb1);
  //M1.print();
  cout<<M1.size()<<"\n";

  MoveList M2(bb2);
  bb2.drawBoard();
  cout<<M2.size()<<"\n";
  
  MoveList M3(bb3);
  bb3.drawBoard();
  cout<<M3.size()<<"\n";

  MoveList M4(bb4);
  bb4.drawBoard();
  cout<<M4.size()<<"\n";

  MoveList M5(bb5);
  bb5.drawBoard();
  cout<<M5.size()<<"\n";

  MoveList M6(bb6);
  bb6.drawBoard();
  cout<<M6.size()<<"\n";  
  
}



void test3()
{
  printf("%d %d %d %d %d %d\n",e4, e1, a1, a2, h1, h2);
  string s;
  while(cin>>s)
    cout<<strToSq(s)<<"\n";
}



void test4()
{
  UndoInfo u1;
  UndoInfo u2(u1);
  UndoInfo u3(u2);
  cout<<u2.halfmoves<<" "<<u3.halfmoves;
}

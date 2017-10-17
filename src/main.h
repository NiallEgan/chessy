#ifndef CHESSY_MAIN_H
#define CHESSY_MAIN_H

#include <memory.h>
#include "board.h"
#include "move.h"

// Macros for a stack, where the pointer stack points to the next free element
#define PUSH(stack, element) *(stack)++ = (element)
#define POP(stack) *(--(stack) - 1)
#define MAX_MOVE_TIME 10


void end();
void end_board(Board);
void init(void);

char* move_to_short_algebraic(Move);
void print_bb(Bitboard);
void print_board(Board);
char * long_algebraic(Move);


int max_depth;

Board fen_to_board(char *);
char *board_to_fen(Board);
Move pfind_move(Board, int, int);

int test_move(char *, int, char *, int);
void test_search(void);
int checkmate_puzzle(char *);

int* copy_ints(int* src, int len);

int run_all_search_tests();

int n_pawn_hits;
int n_pawn_total;
int n_tt_hits;
int n_tt_total;

void take_action(const char*, Board *);

#endif //CHESSY_MAIN_H

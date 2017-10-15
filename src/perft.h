#ifndef CHESSY_PERFT_H
#define CHESSY_PERFT_H

#include "move.h"
#include "board.h"

uint64_t perft(int, struct CBoard *, int, int);
int run_all_tests();

#endif //CHESSY_PERFT_H

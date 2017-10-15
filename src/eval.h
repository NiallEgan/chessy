

#ifndef CHESSY_EVAL_H
#define CHESSY_EVAL_H
#include "move.h"
#include "board.h"

#include <limits.h>

#define RANK(sq) ((sq) / 8)
#define FILE(sq) ((sq) % 8)

#define CHECKMATE (SHRT_MAX / 2)

int pawn_eval(Board, int);

Bitboard front_attack_spans[2][64] ;

double eval(Board, int);

#endif //CHESSY_EVAL_H

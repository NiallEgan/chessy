#ifndef CHESSY_SEARCH_H
#define CHESSY_SEARCH_H

#include "move.h"
#include "board.h"
#include "eval.h"

#include <stdint.h>
#include <time.h>

#define MAX_TABLE_SIZE (262144 * 8)
#define PAWN_TABLE_SIZE 65536
#define INF (SHRT_MAX / 2)

struct Tt_entry {
    Move best_move;
    uint64_t full_hash;
};

struct Pt_entry {
    int val;
    uint64_t full_hash;
};

Move find_move(Board, int);

struct Tt_entry **tt;
struct Pt_entry **pt;
void tt_free(void);
void tt_init(void);
void pt_init(void);
void pt_free(void);

int zobrist_sq_keys[64][12];
int zobrist_c_keys[4];
int zobrist_ep_keys[8];

int pt_keys[64][2];

uint64_t tt_entry(Board, Move, Move, double);
uint64_t pt_entry(Board, int);

uint64_t zobrist_hash(Board);
uint64_t pawn_hash(Board);

Move *move_sort(Move *, Board, int);

double quiescient_search(double, double, Board, int, time_t, time_t, int);
int quiescient_ordering(Move *, size_t);

#endif //CHESSY_SEARCH_H

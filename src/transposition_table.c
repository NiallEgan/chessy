#include "search.h"
#include <stdlib.h>

void tt_init(void) {
	srand((unsigned int) time(NULL));

    for(int i = 0; i < 64; i++) {
		for(int j = 0; j < 12; j++) {
				zobrist_sq_keys[i][j] = rand(); // random enough
		}
	}

    for(int i = 0; i < 4; i++) {
		zobrist_c_keys[i] = rand();
	}

    for(int i = 0; i < 8; i++) {
		zobrist_ep_keys[i] = rand();
	}

	tt = malloc(sizeof(struct Tt_entry *) *  MAX_TABLE_SIZE);

	for(int i = 0; i < MAX_TABLE_SIZE; i++) {
		tt[i] = malloc(sizeof(struct Tt_entry));
		tt[i]->full_hash = 0;
		tt[i]->best_move = 0;
	}
}

void pt_init(void) {
	srand((unsigned int) time(NULL));

    for(int i = 0; i < 64; i++) {
		for(int j = 0; j < 2; j++) {
			pt_keys[i][j] = rand();
		}
	}

    pt = malloc(sizeof(struct Tt_entry *) *  MAX_TABLE_SIZE);
    for(int i = 0; i < MAX_TABLE_SIZE; i++) {
        pt[i] = malloc(sizeof(struct Tt_entry));
        pt[i]->full_hash = 0;
        pt[i]->val = 0;
    }
}


uint64_t tt_entry(Board b, Move killer_move, Move best_move, double val) {
	/* Replaces with newest first. TODO: Experiment with different replacement algorithms */

	uint64_t hash = zobrist_hash(b);
	uint64_t key = hash % MAX_TABLE_SIZE;
	tt[key]->best_move = best_move;
	tt[key]->full_hash = hash;
	return key;
}

uint64_t pt_entry(struct CBoard *b, int val) {
	uint64_t hash = pawn_hash(b);
	uint64_t key = hash % PAWN_TABLE_SIZE;
	pt[key]->val = val;
	pt[key]->full_hash = hash;
	return key;
}

void tt_free(void) {
	for(int i = 0; i < MAX_TABLE_SIZE; i++) {
		if(tt[i] != NULL) free(tt[i]);

	}
	free(tt);
}

void pt_free(void) {
	for(int i = 0; i < PAWN_TABLE_SIZE; i++) {
		if(pt[i] != NULL) free(pt[i]);

	}
	free(pt);
}

uint64_t pawn_hash(Board b) {
	uint64_t h = 0;

	for(int i = 0; i < 64; i++) {
		if(b->superbp[i] == PAWN) {
			h ^= pt_keys[i][b->superbs[i]];
		}
	}

	return h;
}

uint64_t zobrist_hash(Board b) {
	uint64_t h = 0;

	for(int i = 0; i < 64; i++) {
		if(b->superbp[i] != NULL_PIECE) {
			h ^= zobrist_sq_keys[i][b->superbp[i] + 6 * ((b->superbs[i] == -1) ? 0 : 1)];
		}
	}

	for(int i = 0; i < 4; i++) {
		if(b->castling[i]) h ^= zobrist_c_keys[i];
	}

	if(b->ep) h ^= zobrist_ep_keys[b->ep];

    return h;
}
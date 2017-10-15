#ifndef CHESSY_MOVE_H
#define CHESSY_MOVE_H

#include <stdint.h>
#include "board.h"

#define UP_OR_DOWN(side) ? -1 : 1
#define KING_SIDE 2
#define QUEEN_SIDE 3

typedef uint32_t Move;

/* Structure of a Move:
	0 - side
	1-3 - piece type
	4-6 - captured type
	7-9 - promotion type
	10-15 - from
	16-21 - to
	22-23 - Castling, and king or queen side */

unsigned int get_side(Move);
unsigned int get_piece(Move);
unsigned int get_captured(Move);
unsigned int get_promotion(Move);
unsigned int get_from(Move);
unsigned int get_to(Move);
unsigned int get_castling(Move);
unsigned int get_ep(Move);
Move set_ep(unsigned int);

Move create_move(unsigned int, unsigned int, unsigned int,
                 unsigned int, unsigned int, unsigned int,
                 unsigned int);


Move make_move(unsigned int, unsigned int, struct CBoard *);

Bitboard ray_attacks[8][64];

Bitboard get_ray_attacks(struct CBoard *, int, int);
Bitboard get_ray_attacks_forward(struct CBoard *, int, int);
Bitboard get_ray_attacks_backward(struct CBoard *, int, int);


void take_move(Move, struct CBoard *);
void untake_move(Move, struct CBoard *);

int rook_move_gen(struct CBoard *, unsigned int, Move *);
int queen_move_gen(struct CBoard *, unsigned int, Move *);
int bishop_move_gen(struct CBoard *, unsigned int, Move *);
int pawn_move_gen(struct CBoard *, unsigned int, Move *);
int knight_move_gen(struct CBoard *, unsigned int, Move *);
int king_move_gen(struct CBoard *, unsigned int, Move *);

int move_generation(struct CBoard *, unsigned int, Move *);

Bitboard in_check(int, Board);
int in_checkmate(Board);
char *sqs[64];

Move ep_move(unsigned int, unsigned int);

int pseudo_legal(Move, Board);
int in_move_list(Move, Move *, int);

#endif //CHESSY_MOVE_H

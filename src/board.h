#ifndef CHESSY_BOARD_H
#define CHESSY_BOARD_H

#include <stdint.h>

enum directions {N, S, E, W, NE, NW, SE, SW};
enum sides {WHITE, BLACK, NULL_SIDE};

#define WHITE_BOARD b->pieces[13]
#define BLACK_BOARD b->pieces[14]
#define NE_ONE(bb) (((bb) & ~file[7]) >> 9)
#define NW_ONE(bb) (((bb) & ~file[0]) >> 7)
#define SE_ONE(bb) (((bb) & ~file[7]) << 7)
#define SW_ONE(bb) (((bb) & ~file[0]) << 9)
#define SET_BIT(index) (((Bitboard) 1) << (63 - (index)))



int* ep_squares; // a global stack of en passant target squares. Points to the next empty space
int** castling_squares;// a global stack of castling squares


typedef uint64_t Bitboard; // Square a1 = MSB, square h8 = LSB

struct CBoard {
    Bitboard pieces[15]; // first one is null... - not very clever
    unsigned int superbp[64]; // Allows for quick capturing - all the pieces
    unsigned int superbs[64]; // All the sides //
    unsigned int castling[4]; // Can castle in king white, queen white, king black, queen black
    unsigned int have_castled[4]; // If we have castled in the above place
    int ep; // en-passant target square
    int move_count;
    int hm_clock; // The half-move clock
    int pop_count[15]; // The number of each pieces left
    int side_to_move;
    int prev_ep; // For unmake move
};

typedef enum {
    NULL_PIECE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING } Pieces;

typedef struct CBoard * Board;

typedef enum {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
} Square_index;

Bitboard knight_attacks[64];  // The bitboard showing the attacked squares for a knight in any given position

int bit_scan_forwards(Bitboard);
int bit_scan_backwards(Bitboard);
int pop_lsb(Bitboard *);

Bitboard attacked(int, int, Board);

Bitboard file[8];
Bitboard rank[8];

Board deep_copy(Board);
int deep_compare(Board, Board);

void free_board(Board);

#endif //CHESSY_BOARD_H

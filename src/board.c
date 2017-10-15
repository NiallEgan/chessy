#define _GNU_SOURCE

#include "board.h"
#include "move.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <limits.h>


#define debruijn64 0x03f79d71b4cb0a89

#if INT_MAX == 2147483647
#define FFS ffsl
#define CNTLZ __builtin_clzl
#else
#define FFS ffs
#define CNTLZ __builtin_clz
#endif

/* Some useful functions for manipulating boards */

void free_board(Board b) {
    // Frees all the memory of a board
    free(b->superbs);
    free(b->superbp);
    free(b->pieces);
    free(b);
}

const int index64[] = {
        0,  1, 48,  2, 57, 49, 28,  3,
        61, 58, 50, 42, 38, 29, 17,  4,
        62, 55, 59, 36, 53, 51, 43, 22,
        45, 39, 33, 30, 24, 18, 12,  5,
        63, 47, 56, 27, 60, 41, 37, 16,
        54, 35, 52, 21, 44, 32, 23, 11,
        46, 26, 40, 15, 34, 20, 31, 10,
        25, 14, 19,  9, 13,  8,  7,  6
};

Bitboard attacked(int sq, int side, Board b) {
    // Returns 0 if not attacked, the bitboard containing the attacking pieces otherwise

    Bitboard square_bitboard = SET_BIT(sq);

    int side_modifier = 6 * !side; // The amount to add to base piece to get the correct number for the (opposite) side


    Bitboard lateral_moving_pieces = b->pieces[ROOK + side_modifier] | b->pieces[QUEEN + side_modifier];  // i.e. R & Q
    Bitboard lateral_attacks = (get_ray_attacks_forward(b, N, sq) |
                                get_ray_attacks_backward(b, S, sq) |
                                get_ray_attacks_forward(b, E, sq) |
                                get_ray_attacks_backward(b, W, sq)) & lateral_moving_pieces;

    Bitboard diagonal_moving_pieces = b->pieces[BISHOP + side_modifier] | b->pieces[QUEEN + side_modifier];
    Bitboard diagonal_attacks =  (get_ray_attacks_forward(b, NW, sq) |
                                  get_ray_attacks_backward(b, SW, sq) |
                                  get_ray_attacks_forward(b, NE, sq) |
                                  get_ray_attacks_backward(b, SE, sq)) & diagonal_moving_pieces;

    Bitboard l_attacks = knight_attacks[sq] & b->pieces[KNIGHT + side_modifier];

    Bitboard pawn_attacks;

    if (side == WHITE) {
        // Check to see if there is a pawn up and right (can't do so from the rightmost file)
        // and then check to see if there is a pawn up and left
        pawn_attacks = NE_ONE(square_bitboard) | NW_ONE(square_bitboard);
    } else {
        pawn_attacks = SW_ONE(square_bitboard) | SE_ONE(square_bitboard);
    }

    pawn_attacks &= b->pieces[PAWN + side_modifier];

    Bitboard king_attacks = ((square_bitboard >> 8 | square_bitboard << 8) | // North and south
                             NE_ONE(square_bitboard) | NW_ONE(square_bitboard) | SE_ONE(square_bitboard) | SW_ONE(square_bitboard) |
                            ((square_bitboard & ~file[0]) << 1) | ((square_bitboard & ~file[7]) >> 1)) & b->pieces[KING + side_modifier]; // West and east

    return lateral_attacks | diagonal_attacks | l_attacks | pawn_attacks | king_attacks;

}

Board deep_copy(Board b) {
    Board copy = malloc(sizeof(struct CBoard));

    int i;

    for(i = 0; i < 15; i++) { // Copy piece bitboards and population counts
        copy->pieces[i] = b->pieces[i];
        copy->pop_count[i] = b->pop_count[i];
    }

    for(i = 0; i < 64; i++) {
        copy->superbp[i] = b->superbp[i];
        copy->superbs[i] = b->superbs[i];
    }

    for(i = 0; i < 4; i++) {
        copy->castling[i] = b->castling[i];
    }
    copy->ep = b->ep;
    copy->move_count = b->move_count;
    copy->hm_clock = b->hm_clock;
    copy->side_to_move = b->side_to_move;
    copy->prev_ep = b->prev_ep;

    return copy;
}

int deep_compare(Board b1, Board b2) {
    // Checks for equality of two boards
    int i;

    for(i = 1; i < 15; i++) {
        if(b1->pieces[i] != b2->pieces[i]) {
            printf("Pieces: %d\n", i);
            return 0;
        }

        else if(b1->pop_count[i] != b2->pop_count[i]) {
            printf("Pop count: %d\n", i);
            return 0;
        }
    }

    for(i = 0; i < 64; i++) {
        if(b1->superbp[i] != b2->superbp[i]) {
            printf("Superbp: %d\n", i);
            return 0;
        }

        if(b1->superbs[i] != b2->superbs[i]) {
            printf("Superbs: %d\n", i);
            return 0;

        }
    }

    if(b1->ep != b2->ep) {
        printf("ep\n");
        return 0;
    }

    if(b1->move_count != b2->move_count){
        printf("move count\n");
        return 0;
    }

    if(b1->hm_clock != b2->hm_clock) {
        printf("hm\n");
        return 0;
    }

    if(b1->side_to_move != b2->side_to_move) {
        printf("side to move\n");
        return 0;

    }

    if(b1->prev_ep != b2->prev_ep) {
        printf("prev ep");
        return 0;
    }

    return 1;
}

int bit_scan_backwards_software(Bitboard board) {
    // A version of bit_scan_forwards for non-linux platforms
    return 63 - index64[((board & -board) * debruijn64) >> 58];
}

int bit_scan_backwards(Bitboard board) {
    return 64 - FFS(board);
}


int bit_scan_forwards_software(Bitboard b) {
    // A version of bit_scan_forwards for non-linux platforms

    const int rindex64[64] = {
           0, 47,  1, 56, 48, 27,  2, 60,
           57, 49, 41, 37, 28, 16,  3, 61,
           54, 58, 35, 52, 50, 42, 21, 44,
           38, 32, 29, 23, 17, 11,  4, 62,
           46, 55, 26, 59, 40, 36, 15, 53,
           34, 51, 20, 43, 31, 22, 10, 45,
           25, 39, 14, 33, 19, 30,  9, 24,
           13, 18,  8, 12,  7,  6,  5, 63
    };

   // algorithm by Kim Walisch, Mark Dickinson, Eric Cole
   b |= b >> 1;
   b |= b >> 2;
   b |= b >> 4;
   b |= b >> 8;
   b |= b >> 16;
   b |= b >> 32;
   return 63 - rindex64[(b * debruijn64) >> 58];
}

int bit_scan_forwards(Bitboard b) {
    // Return the index of the msb (i.e. floor(lg(n))
    return CNTLZ(b);
}


int pop_lsb_software(Bitboard *board)  {
    Bitboard b = *board & -*board;
    *board &= *board - 1;
	b *= debruijn64;
	b >>= 58;
	return 63 - index64[b];

}

int pop_lsb(Bitboard *board) {
    // Finds index of the lsb (index on the chess board, so reversed)

    int n = 64 - FFS(*board);
    *board &= *board - 1;
    return n;
}
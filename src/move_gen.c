#define _GNU_SOURCE

#include "move.h"
#include <string.h>
#include <stdio.h>
#include <valgrind/memcheck.h>

int move_generation(Board b, unsigned int side, Move *moves) {
    int i = 0;
    i += knight_move_gen(b, side, moves + i);
    i += rook_move_gen(b, side, moves + i);
    i += bishop_move_gen(b, side, moves + i);
    i += queen_move_gen(b, side, moves + i);
    i += pawn_move_gen(b, side, moves + i);
    i += king_move_gen(b, side, moves + i);
    moves += i;


    /* The conditions for castling are as follows:
         * 1. Check that none of the squares in between king and rook (king_side) are not occupied
         * 2. Check that the rook has not been taken - may not be necessary, check
         * 3. Check that the king is not in check
         * 4. And that the king will not be moving through check
         * */

    if(b->castling[0 + 2 * side]) { // king-side
        int king_squares[] = {f1 + 56 * side, g1 + 56 * side};
        Bitboard king_side = SET_BIT(king_squares[0]) | SET_BIT(king_squares[1]);

        if(!(king_side & (BLACK_BOARD | WHITE_BOARD)) && (b->pieces[ROOK + 6 * side] & SET_BIT(h1 + 56 * side))) {

            if(!in_check(side, b) && !attacked(king_squares[0], side, b) && !attacked(king_squares[1], side, b)) {
                *moves++ = create_move(side, NULL_PIECE, NULL_PIECE,NULL_PIECE, e1 + 56 * side, g1 + 56 * side, KING_SIDE);
                i++;
            }
        }
    }

    if(b->castling[1 + 2 * side]) { // queen-side
        int queen_squares[3] = {c1 + 56 * side, d1 + 56 * side, b1 + 56 * side};
        Bitboard queen_side = SET_BIT(queen_squares[0]) | SET_BIT(queen_squares[1]) | SET_BIT(queen_squares[2]);

        if(!(queen_side & (BLACK_BOARD | WHITE_BOARD)) && (b->pieces[ROOK + 6 * side] & SET_BIT(a1 + 56 * side))) {

            if(!in_check(side, b) && !attacked(queen_squares[0], side, b) && !attacked(queen_squares[1], side, b)) {
                *moves = create_move(side, NULL_PIECE, NULL_PIECE, NULL_PIECE, e1 + 56 * side, c1 + 56 * side, QUEEN_SIDE);
                i++;
            }
        }
    }
    return i;
}

int king_move_gen(Board b, unsigned int side, Move *moves) {
    // Returns the number of possible moves the side king can make, and creates a move list.
    // N.B. does not take into account check

    int n_moves = 0;
    Bitboard positions = b->pieces[KING + 6 * side];

    int from = pop_lsb(&positions);
    Bitboard current_king = SET_BIT(from);

    /* 1. Move king one square north
     * 2. Move king one square south
     * 3. Move king NE, NW, SW and SE one square
     * 4. Move king west one square (can't do so if on west most file already)
     * 5. Move king east one square (can't do so if on east most file already)
     * 6. Remove all the squares that are currently occupied by a piece of its own side
     * */

    Bitboard moveb = (current_king >> 8 | current_king << 8 |
                      NE_ONE(current_king) | NW_ONE(current_king) | SW_ONE(current_king) | SE_ONE(current_king) |
                     ((current_king & ~file[0]) << 1) |
                     ((current_king & ~file[7]) >> 1)) &
                     ~b->pieces[13+side];

    while(moveb) {
        *moves++ = make_move((unsigned int) from, (unsigned int) pop_lsb(&moveb), b);
        n_moves++;
    }

    return n_moves;

}

int knight_move_gen(Board b, unsigned int side, Move *moves) {
    int n_moves = 0;
    Bitboard positions = b->pieces[KNIGHT + 6 * side];

    while(positions) {
        int from = pop_lsb(&positions);
        Bitboard moveb = knight_attacks[from] & ~b->pieces[13+side];

        while(moveb) {
            *moves++ = make_move((unsigned int) from, (unsigned int) pop_lsb(&moveb), b);
            n_moves++;
        }
    }

    return n_moves;
}

int pawn_move_gen(Board b, unsigned int side, Move *moves) {
    int n_moves = 0;

    Bitboard all_pieces = BLACK_BOARD | WHITE_BOARD;
    if(side == WHITE) {
        Bitboard positions = b->pieces[PAWN];

        while(positions) { // white
            unsigned int from = (unsigned int) pop_lsb(&positions);
            Bitboard fromboard = SET_BIT(from);

            /* 1. Move forward pawns one square, not onto pieces
             * 2. Move pieces on the 2nd rank two squares forwards, as long as they're not blocked
             * 3. Make north east captures
             * 4. Make north west captures
             *  */

            Bitboard moveb = (fromboard >> 8) & ~all_pieces;
            moveb |= (((fromboard & rank[1] & ~(all_pieces << 8))) >> 16) & ~all_pieces;
            moveb |= NE_ONE(fromboard) & BLACK_BOARD; // NE
            moveb |= NW_ONE(fromboard) & BLACK_BOARD; // NW

            // Must treat moves and promotions separately

            Bitboard promotions = moveb & rank[7];
            moveb &= ~rank[7];

            while(promotions) {
                unsigned int to = (unsigned int) pop_lsb(&promotions);
                *moves++ = create_move(WHITE, PAWN, b->superbp[to], QUEEN, from, to, 0);
                *moves++ = create_move(WHITE, PAWN, b->superbp[to], KNIGHT, from, to, 0);
                *moves++ = create_move(WHITE, PAWN, b->superbp[to], ROOK, from, to, 0);
                *moves++ = create_move(WHITE, PAWN, b->superbp[to], BISHOP, from, to, 0);
                n_moves += 4;
            }


            while(moveb) {
                *moves++ = make_move(from, (unsigned int) pop_lsb(&moveb), b);
                n_moves++;
            }

            // en-passant
            if(SET_BIT(b->ep) & rank[5]) {
                if(from == a5 && b->ep == b6) {
                    *moves++ = ep_move(from, b6);
                    n_moves++;
                } else if(from == h5 && b->ep == g6) {
                    *moves++ = ep_move(from, g6);
                    n_moves++;
                } else if((from >= b5 && from <= g5) && (b->ep == from + 9 || b->ep == from + 7)) {
                    *moves++ = ep_move(from, (unsigned int) b->ep);
                    n_moves++;
                }
            }
        }
    } else {
        Bitboard positions = b->pieces[PAWN + 6];

        while(positions) {

            unsigned int from = (unsigned int) pop_lsb(&positions);
            Bitboard fromboard = SET_BIT(from);

            /* 1. Move forward pawns one square, not onto pieces
             * 2. Move pieces on the 7th rank two squares forwards, as long as they're not blocked
             * 3. Make south east captures
             * 4. Make south west captures
             *  */

            Bitboard moveb = (fromboard << 8) & ~all_pieces;
            moveb |= (((fromboard & rank[6] & ~(all_pieces >> 8))) << 16) & ~all_pieces;
            moveb |= SW_ONE(fromboard) & WHITE_BOARD; // SW
            moveb |= SE_ONE(fromboard) & WHITE_BOARD; // SE

            // Must treat moves and promotions seperately

            Bitboard promotions = moveb & rank[0];
            moveb &= ~rank[0];

            while(promotions) {
                unsigned int to = (unsigned int) pop_lsb(&promotions);
                *moves++ = create_move(BLACK, PAWN, b->superbp[to], QUEEN, from, to, 0);
                *moves++ = create_move(BLACK, PAWN, b->superbp[to], KNIGHT, from, to, 0);
                *moves++ = create_move(BLACK, PAWN, b->superbp[to], BISHOP, from, to, 0);
                *moves++ = create_move(BLACK, PAWN, b->superbp[to], ROOK, from, to, 0);
                n_moves += 4;
            }

            while(moveb) {
                *moves++ = make_move(from, (unsigned int) pop_lsb(&moveb), b);
                n_moves++;
            }

            if(SET_BIT(b->ep) & rank[2]) {
                if(from == a4 && b->ep == b3) {
                    *moves++ = ep_move(from, b3);
                    n_moves++;
                } else if(from == h4 && b->ep == g3) {
                    *moves++ = ep_move(from, g3);
                    n_moves++;
                } else if((from >= b4 && from <= g4) && (b->ep == from - 9 || b->ep == from - 7)) {
                    *moves++ = ep_move(from, (unsigned int) b->ep);
                    n_moves++;
                }
            }
        }


    }
    return n_moves;
}


Bitboard get_ray_attacks_forward(Board b, int dir, int from) {
    // Gets the ray attacks in a given direction

    Bitboard attacks = ray_attacks[dir][from];
    Bitboard blocked = attacks & (WHITE_BOARD | BLACK_BOARD);

    if(blocked) {
        int blocking_square = bit_scan_forwards(blocked);
        attacks ^= ray_attacks[dir][blocking_square];
    }
    return attacks;
}

Bitboard get_ray_attacks_backward(Board b, int dir, int from) {
    // Gets the ray attacks in a given direction

    Bitboard attacks = ray_attacks[dir][from];
    Bitboard blocked = attacks & (WHITE_BOARD | BLACK_BOARD);

    if(blocked) {
        int blocking_square = bit_scan_backwards(blocked);
        attacks ^= ray_attacks[dir][blocking_square];
    }
    return attacks;
}

int rook_move_gen(Board b, unsigned int side, Move *moves) {
    int n_moves = 0;
    Bitboard positions = b->pieces[ROOK + 6 * side];

    while(positions) {
        unsigned int from = (unsigned int) pop_lsb(&positions);

        Bitboard moveb = get_ray_attacks_forward(b, N, from);
        moveb |= get_ray_attacks_backward(b, S, from);
        moveb |= get_ray_attacks_forward(b, E, from);
        moveb |= get_ray_attacks_backward(b, W, from);
        moveb &= ~(b->pieces[13 + side]);  // Otherwise could take own piece

        while(moveb) {
            *moves++ = make_move(from, (unsigned int) pop_lsb(&moveb), b);
            n_moves++;
        }
    }
    return n_moves;
}

int bishop_move_gen(Board b, unsigned int side, Move *moves) {
    int n_moves = 0;
    Bitboard positions = b->pieces[BISHOP + 6*side];

    while(positions) {
        unsigned int from = (unsigned int) pop_lsb(&positions);
        Bitboard moveb = get_ray_attacks_forward(b, NW, from);
        moveb |= get_ray_attacks_backward(b, SW, from);
        moveb |= get_ray_attacks_forward(b, NE, from);
        moveb |= get_ray_attacks_backward(b, SE, from);
        moveb &= ~(b->pieces[13 + side]);
        // Otherwise could take own piece

        while(moveb) {
            *moves++ = make_move(from, (unsigned int) pop_lsb(&moveb), b);
            n_moves++;
        }
    }
    return n_moves;
}

int queen_move_gen(Board b, unsigned int side, Move *moves) {
    int n_moves = 0;
    Bitboard positions = b->pieces[QUEEN + 6 * side];

    while(positions) {
        unsigned int from = (unsigned int) pop_lsb(&positions);
        Bitboard moveb = get_ray_attacks_forward(b, NW, from);
        moveb |= get_ray_attacks_backward(b, SW, from);
        moveb |= get_ray_attacks_forward(b, NE, from);
        moveb |= get_ray_attacks_backward(b, SE, from);
        moveb |= get_ray_attacks_forward(b, N, from);
        moveb |= get_ray_attacks_backward(b, S, from);
        moveb |= get_ray_attacks_forward(b, E, from);
        moveb |= get_ray_attacks_backward(b, W, from);
        moveb &= ~(b->pieces[13 + side]);
        // Otherwise could take own piece

        while(moveb) {
            *moves++ = make_move(from, (unsigned int) pop_lsb(&moveb), b);
            n_moves++;
        }
    }
    return n_moves;
}
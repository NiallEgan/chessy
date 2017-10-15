#include "board.h"
#include "move.h"
#include "search.h"
#include "main.h"

#include <stdlib.h>
#include <valgrind/memcheck.h>


int in_move_list(Move m, Move *moves, int n_moves) {
    // Returns if move is in moves
    if (m == 0) return 0;  // A common case for hash / killer moves that were not set

    for(int i = 0; i < n_moves; i++) {
        if(!moves[i]) break;
        if(moves[i] == m) return 1;
    }

    return 0;
}


int pseudo_legal(Move m, Board b) {
    // Checks if a move is legal, thoughts of check aside


    if(!m) return 0;
    int from = get_from(m);
    int piece = get_piece(m);
    int side = get_side(m);
    int castling = get_castling(m);

    if(castling == 3) {  // Castle queen side
        int queen_squares[3] = {b1 + 56 * side, c1 + 56 * side, d1 + 56 * side};
        Bitboard queen_side = SET_BIT(queen_squares[0]) | SET_BIT(queen_squares[1]) | SET_BIT(queen_squares[2]);

        if(!(queen_side & (BLACK_BOARD | WHITE_BOARD) )) {
            if(!attacked(queen_squares[0], side, b) &&
               !attacked(queen_squares[1], side, b) &&
               !attacked(queen_squares[2], side, b)) {
                return 1;
            }
        }
        return 0;
    } else if(castling == 2) { // Castle king side
        int king_squares[] = {f1 + 56 * side , g1 + 56 * side};
        Bitboard king_side = SET_BIT(king_squares[0]) | SET_BIT(king_squares[1]);

        if(!(king_side & (BLACK_BOARD | WHITE_BOARD))) {  // If king to to rook is unblocked
            if(!attacked(king_squares[0], side, b) &&
               !attacked(king_squares[1], side, b)) {  // neither squares are attacked
                return 1;
            }
        }
        return 0;
    }

    if(!(SET_BIT(from) & b->pieces[piece + 6 * side])) return 0;
    Move *moves = malloc(1024 * sizeof(Move));


    int result = in_move_list(m, moves, move_generation(b, side, moves));
    free(moves);
    return result;
}


Bitboard in_check(int side, Board b) {
    return attacked(bit_scan_forwards(b->pieces[KING + 6 * side]), side, b);
}


void take_move(Move move, Board b) {
    unsigned int side = get_side(move);
    unsigned int moving_piece = get_piece(move);
    unsigned int captured = get_captured(move);
    unsigned int castling = get_castling(move);
    unsigned int from = get_from(move);
    unsigned int to = get_to(move);
    unsigned int ep = get_ep(move);

    int same_side_offset = 6 * side;

    if(!castling && !(ep & 1)) {
        // capture any pieces
        if(captured) {
            int op_side_offset = 6 * !side;
            b->pieces[captured + op_side_offset] ^= SET_BIT(to);
            b->pieces[13 + !side] ^= SET_BIT(to);
            b->pop_count[captured + op_side_offset]--;
            b->pop_count[13 + !side]--;
        }

        // move piece

        // clear the piece from its current square
        b->superbp[from] = NULL_PIECE;
        b->superbs[from] = NULL_SIDE;
        b->pieces[moving_piece + same_side_offset] ^= SET_BIT(from);

        if (!get_promotion(move)) {
            // put it on the to square
            b->pieces[moving_piece + same_side_offset] ^= SET_BIT(to);
            b->superbp[to] = moving_piece;
        } else { // promote the piece
            unsigned int piece_promoted_to = get_promotion(move);
            b->pieces[piece_promoted_to + same_side_offset] ^= SET_BIT(to);
            b->superbp[to] = piece_promoted_to;
            b->pop_count[moving_piece + same_side_offset]--;
            b->pop_count[piece_promoted_to + same_side_offset]++;
        }

        b->superbs[to] = side;
        b->pieces[13 + side] ^= SET_BIT(from) | SET_BIT(to);

        // clear castling
        if (moving_piece == KING) b->castling[2 * side] = b->castling[1 + 2 * side] = 0;
        else if (moving_piece == ROOK) {
            if (from == a1  || from == a8) b->castling[2 * side + 1] = 0;
            else if (from == h1 || from == h8) b->castling[2 * side] = 0;
        }
    } else if (castling) {
        Bitboard  king_from_to, rook_from_to;

        if (castling == KING_SIDE) {
            king_from_to = SET_BIT(e1 + side * 56) | SET_BIT(g1 + side * 56);
            rook_from_to = SET_BIT(h1 + side * 56) | SET_BIT(f1 + side * 56);
            b->have_castled[2 * side] = 1;
        } else { // queen-side
            king_from_to = SET_BIT(e1 + side * 56) | SET_BIT(c1 + side * 56);
            rook_from_to = SET_BIT(a1 + side * 56) | SET_BIT(d1 + side * 56);
            b->have_castled[2 * side + 1] = 1;

        }

        b->pieces[KING + same_side_offset] ^= king_from_to;
        b->pieces[ROOK + same_side_offset] ^= rook_from_to;
        b->pieces[13 + side] ^= king_from_to | rook_from_to;

        int king_to_sq, rook_from_sq, rook_to_sq;

        if (castling == KING_SIDE) {
            king_to_sq = g1;
            rook_from_sq = h1;
            rook_to_sq = f1;
        } else { // queen side
            king_to_sq = c1;
            rook_from_sq = a1;
            rook_to_sq = d1;
        }

        b->superbp[e1 + 56 * side] = NULL_PIECE;  // Clear king
        b->superbp[king_to_sq + 56 * side] = KING;
        b->superbs[e1 + 56 * side] = NULL_SIDE;
        b->superbs[king_to_sq + 56 * side] = side;

        b->superbp[rook_from_sq + 56 * side] = NULL_PIECE;  // Clear rook
        b->superbp[rook_to_sq + 56 * side] = ROOK;
        b->superbs[rook_from_sq + 56 * side] = NULL_SIDE;
        b->superbs[rook_to_sq + 56 * side] = side;
        b->castling[2 * side] = b->castling[1 + 2 * side] = 0;

    } else if(ep & 1) {
        int capture_sq  = ep >> 1;

        b->pieces[PAWN + 6 * !side] ^= SET_BIT(capture_sq);
        b->pieces[13 + !side] ^= SET_BIT(capture_sq);
        b->pieces[13 + side] ^= SET_BIT(from) | SET_BIT(to);
        b->pieces[PAWN + 6 * side] ^= SET_BIT(from) | SET_BIT(to);

        b->superbp[from] = NULL_PIECE;
        b->superbs[from] = NULL_SIDE;

        b->superbp[to] = PAWN;
        b->superbs[to] = side;

        b->superbs[capture_sq] = NULL_SIDE;
        b->superbp[capture_sq] = NULL_PIECE;
        b->pop_count[PAWN + 6 * !side]--;
    }

    // update half move clock
    if(!captured && moving_piece != PAWN && side) b->hm_clock++;

    if(side) b->move_count++;

    b->side_to_move = !b->side_to_move;

    // update ep squares

    if (moving_piece == PAWN && abs(from - to) == 16) {
        int ep_square = to + (side == WHITE ? -1 : 1) * 8;
        PUSH(ep_squares, ep_square);
        b->ep = ep_square;
    } else {
        b->ep = 0;
        PUSH(ep_squares, 0);
    }

    PUSH(castling_squares, copy_ints((int*) b->castling, 4));
}


void untake_move(Move move, Board b) {
    unsigned int moving_piece = get_piece(move);
    unsigned int side = get_side(move);
    unsigned int from = get_from(move);
    unsigned int to = get_to(move);
    unsigned int captured = get_captured(move);
    unsigned int castling = get_castling(move);
    unsigned int ep = get_ep(move);

    int same_side_offset = 6 * side;

    if(!castling && !(ep & 1)) {
        // uncapture any pieces
        if(captured) {
            int op_side_offset = 6 * !side;

            b->pieces[captured + op_side_offset] ^= SET_BIT(to);
            b->pieces[13 + !side] ^= SET_BIT(to);
            b->pop_count[captured + op_side_offset]++;
            b->pop_count[13 + !side]++;
            b->superbp[to] = captured;
            b->superbs[to] = (unsigned int) !side;
        } else {
            b->superbp[to] = NULL_PIECE;
            b->superbs[to] = NULL_SIDE;
        }

        // remove from current square
        if(!get_promotion(move)) {
            b->pieces[moving_piece + same_side_offset] ^= SET_BIT(from) | SET_BIT(to);
        } else {
            b->pieces[get_promotion(move) + same_side_offset] ^= SET_BIT(to);
            b->pieces[PAWN + same_side_offset] ^= SET_BIT(from);
            b->pop_count[PAWN + same_side_offset]++;
            b->pop_count[get_promotion(move) + same_side_offset]--;
        }

        b->superbp[from] = moving_piece;
        b->superbs[from] = side;
        b->pieces[13 + side] ^= SET_BIT(from) | SET_BIT(to);

    } else if(castling) {
        Bitboard  king_from_to, rook_from_to;
        if (castling == KING_SIDE) {
            king_from_to = SET_BIT(e1 + side * 56) | SET_BIT(g1 + side * 56);
            rook_from_to = SET_BIT(h1 + side * 56) | SET_BIT(f1 + side * 56);
            b->have_castled[2 * side] = 0;
        } else { // queen-side
            king_from_to = SET_BIT(e1 + side * 56) | SET_BIT(c1 + side * 56);
            rook_from_to = SET_BIT(a1 + side * 56) | SET_BIT(d1 + side * 56);
            b->have_castled[2 * side + 1] = 0;
        }

        b->pieces[KING + same_side_offset] ^= king_from_to;
        b->pieces[ROOK + same_side_offset] ^= rook_from_to;
        b->pieces[13 + side] ^= king_from_to | rook_from_to;

        int king_to_sq, rook_from_sq, rook_to_sq;

        if (castling == KING_SIDE) {
            king_to_sq = g1;
            rook_from_sq = h1;
            rook_to_sq = f1;
        } else { // queen side
            king_to_sq = c1;
            rook_from_sq = a1;
            rook_to_sq = d1;
        }

        b->superbp[e1 + 56 * side] = KING;  // replace king
        b->superbp[king_to_sq + 56 * side] = NULL_PIECE;
        b->superbs[e1 + 56 * side] = side;
        b->superbs[king_to_sq + 56 * side] = NULL_SIDE;

        b->superbp[rook_from_sq + 56 * side] = ROOK; // replace rook
        b->superbp[rook_to_sq + 56 * side] = NULL_PIECE;
        b->superbs[rook_from_sq + 56 * side] = side;
        b->superbs[rook_to_sq + 56 * side] = NULL_SIDE;
    } else if(ep & 1) {
        int capture_sq  = ep >> 1;
        b->pieces[PAWN + 6 * !side] ^= SET_BIT(capture_sq);
        b->pieces[PAWN + 6 * side] ^= SET_BIT(from) | SET_BIT(to);
        b->pieces[13 + !side] ^= SET_BIT(capture_sq);
        b->pieces[13 + side] ^= SET_BIT(from) | SET_BIT(to);

        b->superbp[from] = PAWN;
        b->superbs[from] = side;

        b->superbp[to] = NULL_PIECE;
        b->superbs[to] = NULL_SIDE;

        b->superbs[capture_sq] = (unsigned int) !side;
        b->superbp[capture_sq] = PAWN;
        b->pop_count[PAWN + 6 * !side]++;
    }

    // update half move clock
    if(!get_captured(move) && moving_piece != 1 && side) b->hm_clock--;

    if(side) b->move_count--;

    b->side_to_move = !b->side_to_move;

    free(*(castling_squares-1));

    int* castle = POP(castling_squares);

    for(int i = 0; i < 4; i++) {
        b->castling[i] = (unsigned int) castle[i];
    }

    b->ep = POP(ep_squares);

}
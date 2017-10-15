#include "move.h"
#include <stdio.h>

// get_side: return the 0th bit
unsigned int get_side(Move move) {
    return move & 1;
}

// get_piece: return the 1st, 2nd and 3rd bits
unsigned int get_piece(Move move) {
    return (move >> 1) & 0x7;
}

// get_captured: get the 4-6 bits
unsigned int get_captured(Move move) {
    return (move >> 4) & 0x7;
}

// get_promotion: gets the 7-9 bits
unsigned int get_promotion(Move move) {
    return (move >> 7) & 0x7;
}


// get_from: gets the 10-15 bits
unsigned int get_from(Move move) {
    return (move >> 10) & 0x3f;
}

// get_to: gets the 16-21 bits
unsigned int get_to(Move move) {
    return (move >> 16) & 0x3f;
}

// get_castling: gets the 22-23 bits
unsigned int get_castling(Move move) {
    return (move >> 22) & 0x3;
}

unsigned int get_ep(Move move) {
    return (move < 1597487104) ? (move >> 24) & 0x7f : 0;

}

Move set_ep(unsigned int capture_sq) {
    Move move = 0;
    return (move | (capture_sq << 25) | (1 << 24));
}

// create_move: returns a complete move, with all information
Move create_move(unsigned int side, unsigned int piece, unsigned int captured, unsigned int promotion, unsigned int from,
                 unsigned int to, unsigned int castling) {
    return side | (piece << 1) | (captured << 4) | (promotion << 7) | (from << 10) | (to << 16) | (castling << 22);
}

// make_move: returns a complete move, using the from and to square

Move make_move(unsigned int from, unsigned int to, Board b) {

    Move m = create_move(b->superbs[from], b->superbp[from], b->superbp[to], NULL_PIECE, from, to, 0);
    return m;
}

Move ep_move(unsigned int from, unsigned int to) {
    if(!(from / 8 == 3 || from / 8 == 4)) {
        printf("EP Err;%s, %s\n", sqs[from], sqs[to]);
    }
    unsigned int side = (a5 <= from) ? WHITE : BLACK;

    return create_move(side, 0, 0, 0, from, to, 0) | set_ep(to + (side == WHITE ? -1 : 1) * 8);
}

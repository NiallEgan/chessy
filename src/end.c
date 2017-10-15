#include "main.h"
#include "search.h"

#include <stdlib.h>

void end() {
    tt_free();
	pt_free();
    free(castling_squares);
    free(ep_squares);
}

void end_board(Board b) {
    free(b);
    POP(ep_squares);
    free(*(castling_squares - 1));
    POP(castling_squares);
}
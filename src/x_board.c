#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "main.h"
#include "search.h"

int go_side = BLACK;
int force = 0;
int board_made = 0;

Move atomove(char const *squares, Board b) {
    if (strlen(squares) != 4 && strlen(squares) != 5) return 0;

    if ('a' <= squares[0] && squares [0] <= 'h' && '1' <= squares[1] && squares[1] <= '9'  &&
        'a' <= squares[2] && squares [2] <= 'h' && '1' <= squares[3] && squares[3] <= '9') {

        if (strcmp(squares, "e1g1") == 0) {
            return create_move(b->side_to_move, NULL_PIECE, NULL_PIECE,NULL_PIECE, e1 + 56 * WHITE, g1 + 56 * WHITE, KING_SIDE);
        } else if (strcmp(squares, "e1c1") == 0) {
            return create_move(b->side_to_move, NULL_PIECE, NULL_PIECE,NULL_PIECE, e1 + 56 * WHITE, c1 + 56 * WHITE, QUEEN_SIDE);
        } else if (strcmp(squares, "e8g8") == 0) {
            return create_move(b->side_to_move, NULL_PIECE, NULL_PIECE,NULL_PIECE, e1 + 56 * BLACK, g1 + 56 * BLACK, KING_SIDE);
        } else if (strcmp(squares, "e8c8") == 0) {
            return create_move(b->side_to_move, NULL_PIECE, NULL_PIECE,NULL_PIECE, e1 + 56 * BLACK, c1 + 56 * BLACK, QUEEN_SIDE);
        }

        if (strlen(squares) == 5) { // Promotion
            unsigned int from =  (unsigned) (squares[0] - 97)  + (squares[1] - 49) * 8;
            unsigned int to = (unsigned) (squares[2] - 97) + (squares[3] - 49) * 8;
            char prom_char = squares[4];
            unsigned int promotion;
            switch (prom_char) {
                case ('n'):
                    promotion = KNIGHT;
                    break;
                case ('b'):
                    promotion = BISHOP;
                    break;
                case ('r'):
                    promotion = ROOK;
                    break;
                case ('q'):
                    promotion = QUEEN;
                    break;
                default:
                    // Should perhaps print something here, but likely to just confuse GUI
                    break;
            }
            return create_move(b->side_to_move, PAWN, b->superbp[to], promotion, from, to, 0);
        } else {
            unsigned int from =  (unsigned) (squares[0] - 97)  + (squares[1] - 49) * 8;
            unsigned int to = (unsigned) (squares[2] - 97) + (squares[3] - 49) * 8;
            return make_move(from, to, b);
        }
    }

    return 0;
}

void take_action(const char *command, Board *bp) {
    printf("IM taking action %s\n", command);


    if (strcmp(command, "new") == 0) {
        init();
        *bp = fen_to_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
        board_made = 1;
    } else if (strcmp(command, "force") == 0) {
        force = 1;
    } else if(strcmp(command, "go") == 0) {
        go_side = (*bp)->side_to_move;
        if (board_made && go_side == (*bp)->side_to_move && !force) {
            Move m = find_move(*bp, MAX_MOVE_TIME);
            take_move(m, *bp);
            printf("move %s\n", long_algebraic(m));
        }
    } else if(strcmp(command, "level") == 0) {
        // TODO
    } else if (strcmp(command, "quit") == 0) {
        end();
        exit(0);
    } else {
        Move m;
        if ((m = atomove(command, *bp))) {
            take_move(m, *bp);

            if (board_made && go_side == (*bp)->side_to_move && !force) {
                Move my_move = find_move(*bp, MAX_MOVE_TIME);
                take_move(my_move, *bp);
                printf("move %s\n", long_algebraic(my_move));
            }
        }
    }
}
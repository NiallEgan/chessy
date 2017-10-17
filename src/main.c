#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "main.h"
#include "search.h"
#include "perft.h"

#define COMPUTER_SIDE WHITE

int* copy_ints(int* src, int len) {
    int* tmp = malloc(sizeof(int) * len);
    memcpy(tmp, src, len * sizeof(int));
    return tmp;
}

// atomove: takes a string and returns a move

int main() {
    Board *bp = malloc(sizeof(Board *));

    FILE *fp = fopen("out.txt", "w");
    fprintf(fp, "out\n");
    fclose(fp);

    setbuf(stdout, NULL);
    signal(SIGINT, SIG_IGN);

    while (1) {
        char command[50];
        scanf("%s", command);
        take_action(command, bp);

    }

    /*init();

    //run_all_tests();

    setbuf(stdout, NULL);

    n_tt_total = n_tt_hits = n_pawn_total = n_pawn_hits = 0;

    Board b = fen_to_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");

    printf("Enter your moves in long algebraic form, i.e. from square to square, e.g. a2a3\n");

    if (COMPUTER_SIDE == BLACK) {
        char user_move[6];
        scanf("%s", user_move);
        take_move(atomove(user_move, b), b);
    }

    while(1) {
        print_board(b);
        Move move = find_move(b, MAX_MOVE_TIME);
        long_algebraic(move);;
        take_move(move, b);
        print_board(b);

        char user_move[6];
        scanf("%s", user_move);
        if (strcmp(user_move, "oo") == 0)  {
            take_move(create_move(b->side_to_move, NULL_PIECE, NULL_PIECE,NULL_PIECE, e1 + 56 * b->side_to_move, g1 + 56 * b->side_to_move, KING_SIDE), b);
        } else if (strcmp(user_move, "ooo") == 0) {
            take_move(create_move(b->side_to_move, NULL_PIECE, NULL_PIECE, NULL_PIECE, e1 + 56 * b->side_to_move, c1 + 56 * b->side_to_move, QUEEN_SIDE), b);
        }
        else if (strcmp(user_move, "end") == 0) break;
        else take_move(atomove(user_move, b), b);
    }

    end();

    printf("TT hit rate: %f\n", (double) n_tt_hits / n_tt_total);
    printf("Total nodes searched (ish): %d\n", n_pawn_total);
    printf("PT hit rate: %0.2f\n", (double) n_pawn_hits / n_pawn_total);*/

    return 0;
}
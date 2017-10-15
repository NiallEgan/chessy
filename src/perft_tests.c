#include <stdlib.h>
#include <stdio.h>
#include "board.h"
#include "main.h"
#include "perft.h"
#include <inttypes.h>
#include <memory.h>

int MAX_DEPTH = 5;

uint64_t perft(int depth, Board b, int side, int max_perft) {
    // Counts the number of moves from a given position up to depth
    //global_depth++;
    if(depth == 0) return 1;

    Move *moves = malloc(512 * sizeof(Move));
    uint64_t nodes = 0;

    int move_no = move_generation(b, side, moves);

    for(int i = 0; i < move_no; i++) {
        take_move(moves[i], b);

        if(!in_check(side, b)) {
            uint64_t current_no = perft(depth - 1, b, !side, max_perft);
            nodes += current_no;

            int x = 0;
            if(depth == max_perft || (x)) {
                for (int j = depth; j < max_perft; j++) printf("     ");
                print_from_to(moves[i]);
                if (depth == 2) {
                }

                printf(": %" PRIu64 "\n", current_no);
            }

        }
        untake_move(moves[i], b);
    }
    free(moves);
    return nodes;
}

int run_test(int test_no) {
    char filename[27];
    sprintf(filename, "../test/perft_tests/%d.test", test_no);

    FILE *fp;
    if((fp = fopen(filename, "r")) == NULL) {
        printf("File read error: %s\n", filename);
        return 0;
    }

    char line[256];

    // Read in fen string
    fgets(line, sizeof(line), fp);

    Board b = fen_to_board(line);


    int depth = 1;
    while(fgets(line, sizeof(line), fp)) {
        if (depth > MAX_DEPTH) break;

        char n[256];
        int i = 0;
        while(line[i++] != ' ') ;

        int j = 0;
        char c;
        while((c = line[i++])) n[j++] = c;

        int result = perft(depth, b, b->side_to_move, depth);

        if(result == atoi(n)) {
            printf("Test %d at depth %d passed\n", test_no, depth);
        } else {
            printf("Test %d at depth %d failed: expected %d, got %d\n", test_no, depth, atoi(n), result);
            return 0;
        }

        depth++;
    }

    return 1;
}

int run_all_tests() {
    for(int i = 1; i < 7; i++) {
        if(!run_test(i)) return 0;
    }

    return 1;
}
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "main.h"
#include "search.h"

int run_search_test(char *epd_string, time_t time) {
    // Read an epd string with an bm and id field
    Board b = fen_to_board(epd_string);
    char* best_move = malloc(sizeof(char) * 10);
    char* id = malloc(sizeof(char) * 50);
    char* best_move_start = best_move;
    char* id_start = id;
    int n_spaces = 0;
    int i = 0;

    for (char c = epd_string[i]; n_spaces < 4; c = epd_string[i++]) {
        if (c == ' ') n_spaces++;  // Read to the end of the fen string
    }

    i--;

    if (epd_string[i] == 'b') { // best move first
        i += 3;

        while (epd_string[i++] != ';') {
            if(epd_string[i-1] != '+' && epd_string[i-1] != '#') *best_move++ = epd_string[i-1];
        }

        *best_move = '\0';
        i += 4;

        while (epd_string[i++] != ';') {
            *id++ = epd_string[i-1];
        }

        *id = '\0';
    } else {
        i += 4;

        while (epd_string[i++] != ';') {
            *id++ = epd_string[i-1];
        }

        *id = '\0';
        i += 4;

        while (epd_string[i++] != ';') {
            if(epd_string[i-1] != '+' && epd_string[i-1] != '#') *best_move++ = epd_string[i-1];
        }

        *best_move = '\0';
    }

    printf("Running test %s\n", id_start);

    Move move = find_move(b, time);
    char* move_string = move_to_short_algebraic(move);

    if (strcmp(move_string, best_move_start) == 0) {
        printf("Test %s passed with move %s \n", id_start, best_move_start);
        free(best_move_start);
        free(id_start);
        end_board(b);
        free(move_string);
        return 1;
    } else {
        printf("Test %s failed: expected %s, got %s\n", id, best_move_start, move_string);
        free(best_move_start);
        free(id_start);
        end_board(b);
        free(move_string);
        return 0;
    }

}

int test_file(char* filename) {
    FILE* fp;

    if((fp = fopen(filename, "r")) == NULL) {
        printf("File read error: %s\n", filename);
        return 0;
    }

    char line[512];

    while(fgets(line, sizeof(line), fp)) {

        if (!run_search_test(line, 15)) {
            return 0;
        }
    }

    return 1;
}

int run_all_search_tests() {
    //if (!test_file("../test/search_tests/bk.test")) return 0;
    //return test_file("../test/search_tests/ccr.test");
    return test_file("../test/search_tests/easy.test");
}

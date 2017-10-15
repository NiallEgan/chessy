#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "board.h"
#include "main.h"
#include "move.h"


void print_from_to(Move move) {
	if(!get_promotion(move)) printf("%s%s", sqs[get_from(move)], sqs[get_to(move)]);
    else {
        char promotion;
        switch(get_promotion(move)) {
            case KNIGHT:
                promotion = 'n';
                break;
            case BISHOP:
                promotion = 'b';
                break;
            case ROOK:
                promotion = 'r';
                break;
            case QUEEN:
                promotion = 'q';
                break;
            default:
                promotion = '?';
                break;
        }
        printf("%s%s%c", sqs[get_from(move)], sqs[get_to(move)], promotion);
    }
}

char* move_to_short_algebraic(Move move) {
    // TODO: Add support for + (check) and # (checkmate)
	char pieces[7] = {'\0', 'P', 'N', 'B', 'R', 'Q', 'K'};
	char files[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

    char* move_string = malloc(sizeof(char) * 10);

	if(get_castling(move)) {
		sprintf(move_string, "%s", (get_castling(move) - 2) ? "O-O-O" : "O-O");
	}

	if(!get_captured(move)) {
		if(get_piece(move) == PAWN) {
			sprintf(move_string, "%s", sqs[get_to(move)]);
		}
		else {
			sprintf(move_string, "%c%s", pieces[get_piece(move)], sqs[get_to(move)]);
		}
	} else {
		if(get_piece(move) == PAWN) {
			sprintf(move_string, "%cx%s", files[get_from(move) % 8],
				  sqs[get_to(move)]);
		} else {
			sprintf(move_string, "%cx%s", pieces[get_piece(move)], sqs[get_to(move)]);
		}
	}

    if (get_promotion(move)) {
        char* promotion = malloc(sizeof(char) * 2);
        sprintf(promotion, "%c", pieces[get_promotion(move)]);
        strcat(move_string, promotion);
        free(promotion);
    }

    return move_string;
}

void print_bb(Bitboard b) {
    for(int rank = 7; rank >= 0; rank--) {
        for(int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (square < 10) printf("%d: ", square);
            else printf("%d:", square);

            printf("%d ",(int) (b >> (63 - square) & 1));
            if((square + 1) % 8 == 0) {
                printf("\n");
            }
        }
    }
	printf("\n");
}

void print_board(struct CBoard *b) {
	char board[64];

    for(int square = 0; square < 64; square++) {
		board[square] = '-';
	}

    char piece_array[7] = {'\0', 'P', 'N', 'B', 'R', 'Q', 'K'};


	for(int square = 0; square < 64; square++) {
		if(b->superbp[square]) {
			if(b->superbs[square] == WHITE) board[square] = piece_array[b->superbp[square]];
			else if(b->superbs[square] == BLACK) board[square] = (char) tolower(piece_array[b->superbp[square]]);
		}
	}

    printf("\n");

    for(int rank = 7; rank >= 0; rank--) {
        for(int file = 0; file < 8; file++) {
            int square = 8 * rank + file;

            if (square < 10) printf(" %d: ", square);
            else printf(" %d:", square);
            printf("%c", board[square]);

            if((square + 1) % 8 == 0) {
                printf("\n");
            }
        }
    }
}


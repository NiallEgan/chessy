#include "board.h"
#include "main.h"

#include <ctype.h>
#include <stdio.h>

void add_piece(int sq, int side, Board b, int piece) {
    b->superbp[sq] = (unsigned int) piece;
    b->superbs[sq] = (unsigned int) side;
    b->pieces[piece + 6 * side] |= SET_BIT(sq);
    b->pop_count[piece + 6 * side]++;
    b->pop_count[13 + side]++;
}


char *board_to_fen(Board b) {
    char digits[] = {'0', '1', '2', '3', '4', '5', '6',
                     '7', '8', '9'};

    char pieces[] = {'p', 'n', 'b', 'r', 'q', 'k'};
    char *fen_string = malloc(sizeof(char) * 80);
    int i = 0;
    int empty_sqs = 0;

    for(int rank_no = 7; rank_no >= 0 ; rank_no--) {
        for(int file_no = 0; file_no < 8; file_no++) {
            int square = rank_no * 8 + file_no;

            if(b->superbs[square] == BLACK) {
                if(empty_sqs) {
                    fen_string[i++] = digits[empty_sqs];
                    empty_sqs = 0;
                }

                fen_string[i++] = pieces[b->superbp[square] - 1];
            } else if(b->superbs[square] == WHITE) {
                if(empty_sqs) {
                    fen_string[i++] = digits[empty_sqs];
                    empty_sqs = 0;
                }

                fen_string[i++] = (char) toupper(pieces[b->superbp[rank_no * 8 + file_no] - 1]); // conversion o.k.
            } else empty_sqs++;

            if(file_no == 7 && rank_no != 0) {
                if(empty_sqs) {
                    fen_string[i++] = digits[empty_sqs];
                    empty_sqs = 0;
                }
                fen_string[i++] = '/';
            }
        }
    }

    fen_string[i++] = ' ';
    fen_string[i++] = (char) ((b->side_to_move) ? 'b' : 'w');
    fen_string[i++] = ' ';

    if(b->castling[0]) fen_string[i++] = 'K';
    if(b->castling[1]) fen_string[i++] = 'Q';
    if(b->castling[2]) fen_string[i++] = 'k';
    if(b->castling[3]) fen_string[i++] = 'q';

    fen_string[i++] = ' ';

    if(b->ep != 0) {
        fen_string[i++] = sqs[b->ep][0];
        fen_string[i++] = sqs[b->ep][1];
    } else fen_string[i++] = '-';

    // half-moves
    fen_string[i++] = ' ';
    char hm[3];
    int k = 0;
    sprintf(hm, "%d", b->hm_clock);
    while((fen_string[i++] = hm[k++])) ;

    // move no
    fen_string[i++] = ' ';
    char move_no[5];
    k = 0;
    sprintf(move_no, "%d", b->move_count);
    while((fen_string[i++] = move_no[k++])) ;
    fen_string[i] = '\0';
    return fen_string;
}

Board fen_to_board(char *fen) {
    char c;
    int number_of_spaces = 0;
    int file_no = 0;
    int rank_no = 7;

    struct CBoard *b = malloc(sizeof(struct CBoard));

    castling_squares = malloc(sizeof(int*) * 512);

    b->castling[0] = b->castling[1] = b->castling[2] =
    b->castling[3] = 0;

    for(int square = 0; square < 64; square++) {
        b->superbp[square] = NULL_PIECE;
        b->superbs[square] = NULL_SIDE;
    }

    for(int piece = 0; piece < 15; piece++) {
        b->pieces[piece] = 0;
        b->pop_count[piece] = 0;
    }

    while((c = *fen++)) {
        if (c == '\n') break;

        if(c == ' ') number_of_spaces++;
        else {
            switch(number_of_spaces) {
                case 0:
                    if(isdigit(c)) {
                        file_no += c - 48; // skip blank spaces
                    }
                    switch(c) {
                        case('/'):
                            rank_no--;  // new file
                            file_no = 0;
                            break;
                        case('P'):
                            add_piece(rank_no * 8 + file_no, WHITE, b, PAWN);
                            file_no++;
                            break;
                        case('p'):
                            add_piece(rank_no * 8 + file_no, BLACK, b, PAWN);
                            file_no++;
                            break;
                        case('N'):
                            add_piece(rank_no * 8 + file_no, WHITE, b, KNIGHT);
                            file_no++;
                            break;
                        case('n'):
                            add_piece(rank_no * 8 + file_no, BLACK, b, KNIGHT);
                            file_no++;
                            break;
                        case('B'):
                            add_piece(rank_no * 8 + file_no, WHITE, b, BISHOP);
                            file_no++;
                            break;
                        case('b'):
                            add_piece(rank_no * 8 + file_no, BLACK, b, BISHOP);
                            file_no++;
                            break;
                        case('R'):
                            add_piece(rank_no * 8 + file_no, WHITE, b, ROOK);
                            file_no++;
                            break;
                        case('r'):
                            add_piece(rank_no * 8 + file_no, BLACK, b, ROOK);
                            file_no++;
                            break;
                        case('Q'):
                            add_piece(rank_no * 8 + file_no, WHITE, b, QUEEN);
                            file_no++;
                            break;
                        case('q'):
                            add_piece(rank_no * 8 + file_no, BLACK, b, QUEEN);
                            file_no++;
                            break;
                        case('K'):
                            add_piece(rank_no * 8 + file_no, WHITE, b, KING);
                            file_no++;
                            break;
                        case('k'):
                            add_piece(rank_no * 8 + file_no, BLACK, b, KING);
                            file_no++;
                            break;
                        default:
                            break;
                    }
                    break;
                case(1):
                    b->side_to_move = (c == 'w') ? WHITE : BLACK;
                    break;
                case(2):
                    switch(c) {
                        case('K'):
                            b->castling[0] = 1;
                            break;
                        case('k'):
                            b->castling[2] = 1;
                            break;
                        case('Q'):
                            b->castling[1] = 1;
                            break;
                        case('q'):
                            b->castling[3] = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                case(3):
                    if(c == '-') {
                        PUSH(ep_squares, 0);
                        b->ep = 0;
                    } else {
                        int sq = c - 97 + (*(fen + 1) - 49) * 8;
                        PUSH(ep_squares, sq);
                        b->ep = sq;
                    }
                    break;
                case(4):
                    b->hm_clock = c - 48;
                    break;
                case(5):
                    b->move_count = c - 48;
                    break;
                default:
                    break;
            }
        }
    }
    WHITE_BOARD = b->pieces[PAWN] | b->pieces[KNIGHT] | b->pieces[BISHOP] | b->pieces[ROOK] | b->pieces[QUEEN] | b->pieces[KING];

    BLACK_BOARD = b->pieces[PAWN + 6] | b->pieces[KNIGHT + 6] | b->pieces[BISHOP + 6] | b->pieces[ROOK + 6] | b->pieces[QUEEN + 6] | b->pieces[KING + 6];

    PUSH(castling_squares, copy_ints((int *) b->castling, 4));

    for (int i = 0; i < 4; i++) b->have_castled[i] = 0;
    return b;
}
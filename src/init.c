#include "board.h"
#include "move.h"
#include "main.h"
#include "search.h"

#include <stdlib.h>
#include "eval.h"

void init(void) {
    max_depth = 0;
    tt_init();
    pt_init();

    ep_squares = calloc(512, sizeof(int));

    int dir, square;
    Bitboard west_to_east_diags[64];
    Bitboard east_to_west_diags[64];

    Bitboard full_board = ~(Bitboard)0;
    Bitboard rank_one =
            SET_BIT(a1) | SET_BIT(b1) | SET_BIT(c1) | SET_BIT(d1) | SET_BIT(e1) | SET_BIT(f1) | SET_BIT(g1) |
            SET_BIT(h1);
    Bitboard file_a = SET_BIT(a1) | SET_BIT(a2) | SET_BIT(a3) | SET_BIT(a4) | SET_BIT(a5) | SET_BIT(a6) | SET_BIT(a7) |
            SET_BIT(a8);

    int i;
    for(i = 0; i < 8; i++) {
        file[i] = file_a >> i;
        rank[i] = rank_one >> 8 * i;
    }

    // Set the nth bit in each byte for each n bytes
    // i.e. make the diagonals
    for(square = 0; square < 64; square++) {
        switch (square) {
            case a1:
                west_to_east_diags[a1] = SET_BIT(a1);
                break;
            case a2:
                west_to_east_diags[a2] = west_to_east_diags[b1] = SET_BIT(a2) | SET_BIT(b1);
                break;
            case a3:
                west_to_east_diags[a3] = west_to_east_diags[b2] = west_to_east_diags[c1] = SET_BIT(a3) | SET_BIT(b2) |
                        SET_BIT(c1);
                break;
            case a4:
                west_to_east_diags[a4] = west_to_east_diags[b3] = west_to_east_diags[c2] = west_to_east_diags[d1] =
                        SET_BIT(a4) |
                        SET_BIT(b3) | SET_BIT(c2) | SET_BIT(d1);
                break;
            case a5:
                west_to_east_diags[a5] = west_to_east_diags[b4] = west_to_east_diags[c3] = west_to_east_diags[d2] = west_to_east_diags[e1] =
                        SET_BIT(a5) | SET_BIT(b4) | SET_BIT(c3) | SET_BIT(d2) |
                                SET_BIT(e1);
                break;
            case a6:
                west_to_east_diags[a6] = west_to_east_diags[b5] = west_to_east_diags[c4] = west_to_east_diags[d3] = west_to_east_diags[e2] = west_to_east_diags[f1] =
                        SET_BIT(a6) | SET_BIT(b5) | SET_BIT(c4) | SET_BIT(d3) |
                                SET_BIT(e2) | SET_BIT(f1);
                break;
            case a7:
                west_to_east_diags[a7] = west_to_east_diags[b6] = west_to_east_diags[c5] = west_to_east_diags[d4] = west_to_east_diags[e3] = west_to_east_diags[f2] =
                west_to_east_diags[g1] = SET_BIT(a7) | SET_BIT(b6) | SET_BIT(c5) |
                        SET_BIT(d4) | SET_BIT(e3) | SET_BIT(f2) | SET_BIT(g1);
                break;
            case a8:
                west_to_east_diags[a8] = west_to_east_diags[b7] = west_to_east_diags[c6] = west_to_east_diags[d5] = west_to_east_diags[e4] = west_to_east_diags[f3] =
                west_to_east_diags[g2] = west_to_east_diags[h1] = SET_BIT(a8) | SET_BIT(b7) |
                        SET_BIT(c6) | SET_BIT(d5) | SET_BIT(e4) | SET_BIT(f3) |
                        SET_BIT(g2) | SET_BIT(h1);
                break;
            case b8:
                west_to_east_diags[b8] = west_to_east_diags[c7] = west_to_east_diags[d6] = west_to_east_diags[e5] = west_to_east_diags[f4] = west_to_east_diags[g3] =
                west_to_east_diags[h2] = SET_BIT(b8) | SET_BIT(c7) | SET_BIT(d6) |
                        SET_BIT(e5) | SET_BIT(f4) | SET_BIT(g3) | SET_BIT(h2);
                break;
            case c8:
                west_to_east_diags[c8] = west_to_east_diags[d7] = west_to_east_diags[e6] = west_to_east_diags[f5] = west_to_east_diags[g4] = west_to_east_diags[h3] =
                        SET_BIT(c8) | SET_BIT(d7) | SET_BIT(e6) | SET_BIT(f5) |
                                SET_BIT(g4) | SET_BIT(h3);
                break;
            case d8:
                west_to_east_diags[d8] = west_to_east_diags[e7] = west_to_east_diags[f6] = west_to_east_diags[g5] = west_to_east_diags[h4] =
                        SET_BIT(d8) | SET_BIT(e7) | SET_BIT(f6) | SET_BIT(g5) |
                                SET_BIT(h4);
                break;
            case e8:
                west_to_east_diags[e8] = west_to_east_diags[f7] = west_to_east_diags[g6] = west_to_east_diags[h5] =
                        SET_BIT(e8) |
                        SET_BIT(f7) | SET_BIT(g6) | SET_BIT(h5);
                break;
            case f8:
                west_to_east_diags[f8] = west_to_east_diags[g7] = west_to_east_diags[h6] = SET_BIT(f8) | SET_BIT(g7) |
                        SET_BIT(h6);
                break;
            case g8:
                west_to_east_diags[g8] = west_to_east_diags[h7] = SET_BIT(g8) | SET_BIT(h7);
                break;
            case h8:
                west_to_east_diags[h8] = SET_BIT(h8);
                break;
            default:
                break;
        }
    }

    for(square = 0; square < 64; square++) {
        switch (square) {
            case h1:
                east_to_west_diags[h1] = SET_BIT(h1);
                break;
            case g1:
                east_to_west_diags[g1] = east_to_west_diags[h2] = SET_BIT(g1) | SET_BIT(h2);
                break;
            case f1:
                east_to_west_diags[f1] = east_to_west_diags[g2] = east_to_west_diags[h3] = SET_BIT(f1) | SET_BIT(g2) |
                        SET_BIT(h3);
                break;
            case e1:
                east_to_west_diags[e1] = east_to_west_diags[f2] = east_to_west_diags[g3] = east_to_west_diags[h4] =
                        SET_BIT(e1) |
                        SET_BIT(f2) | SET_BIT(g3) | SET_BIT(h4);
                break;
            case d1:
                east_to_west_diags[d1] = east_to_west_diags[e2] = east_to_west_diags[f3] = east_to_west_diags[g4] = east_to_west_diags[h5] =
                        SET_BIT(h5) | SET_BIT(g4) | SET_BIT(f3) | SET_BIT(e2) |
                                SET_BIT(d1);
                break;
            case c1:
                east_to_west_diags[c1] = east_to_west_diags[d2] = east_to_west_diags[e3] = east_to_west_diags[f4] = east_to_west_diags[g5] =east_to_west_diags[h6] =
                        SET_BIT(c1) | SET_BIT(d2) | SET_BIT(e3) | SET_BIT(f4) |
                                SET_BIT(g5) | SET_BIT(h6);
                break;
            case b1:
                east_to_west_diags[b1] = east_to_west_diags[c2] = east_to_west_diags[d3] = east_to_west_diags[e4] = east_to_west_diags[f5] = east_to_west_diags[g6] =
                east_to_west_diags[h7] = SET_BIT(b1) | SET_BIT(c2) | SET_BIT(d3) | SET_BIT(e4) | SET_BIT(f5) |
                        SET_BIT(g6) |
                        SET_BIT(h7);
                break;
            case a1:
                east_to_west_diags[a1] = east_to_west_diags[b2] = east_to_west_diags[c3] = east_to_west_diags[d4] = east_to_west_diags[e5] = east_to_west_diags[f6] =
                east_to_west_diags[g7] = east_to_west_diags[h8] =
                        SET_BIT(a1) | SET_BIT(b2) | SET_BIT(c3) | SET_BIT(d4) |
                        SET_BIT(e5) |
                        SET_BIT(f6) |
                        SET_BIT(g7) | SET_BIT(h8);
                break;
            case a2:
                east_to_west_diags[a2] = east_to_west_diags[b3] = east_to_west_diags[c4] = east_to_west_diags[d5] = east_to_west_diags[e6] = east_to_west_diags[f7] =
                east_to_west_diags[g8] = SET_BIT(a2) | SET_BIT(b3) | SET_BIT(c4) | SET_BIT(d5) | SET_BIT(e6) |
                        SET_BIT(f7) |
                        SET_BIT(g8);
                break;
            case a3:
                east_to_west_diags[a3] = east_to_west_diags[b4] = east_to_west_diags[c5] = east_to_west_diags[d6] = east_to_west_diags[e7] = east_to_west_diags[f8] =
                        SET_BIT(a3) | SET_BIT(b4) | SET_BIT(c5) | SET_BIT(d6) | SET_BIT(e7) | SET_BIT(f8);
                break;
            case a4:
                east_to_west_diags[a4] = east_to_west_diags[b5] = east_to_west_diags[c6] = east_to_west_diags[d7] = east_to_west_diags[e8] =
                        SET_BIT(a4) | SET_BIT(b5) | SET_BIT(c6) | SET_BIT(d7) | SET_BIT(e8);
                break;
            case a5:
                east_to_west_diags[a5] = east_to_west_diags[b6] = east_to_west_diags[c7] = east_to_west_diags[d8] =
                        SET_BIT(a5) |
                        SET_BIT(b6) | SET_BIT(c7) | SET_BIT(d8);
                break;
            case a6:
                east_to_west_diags[a6] = east_to_west_diags[b7] = east_to_west_diags[c8] = SET_BIT(a6) | SET_BIT(b7) |
                        SET_BIT(c8);
                break;
            case a7:
                east_to_west_diags[a7] = east_to_west_diags[b8] = SET_BIT(a7) | SET_BIT(b8);
                break;
            case a8:
                east_to_west_diags[a8] = SET_BIT(a8);
                break;
            default:
                break;
        }
    }

    for(dir = 0; dir < 8; dir++) {
        for(square = 0; square < 64; square++)
            switch (dir) {
                case N: // North
                    ray_attacks[dir][square] = (full_board >> square) & file[square % 8] & ~SET_BIT(square);
                    break;
                case S: // South
                    ray_attacks[dir][square] = (full_board << (63 - square)) & file[square % 8] & ~SET_BIT(square);
                    break;
                case E: // East
                    ray_attacks[dir][square] = (full_board >> square) & rank[square / 8] & ~SET_BIT(square);
                    break;
                case W: // West
                    ray_attacks[dir][square] = (full_board << (63 - square)) & rank[square / 8] & ~SET_BIT(square);
                    break;
                case NE: // North East
                    ray_attacks[dir][square] = (full_board >> square) & east_to_west_diags[square] & ~SET_BIT(square);
                    break;
                case NW: // North West
                    ray_attacks[dir][square] = (full_board >> square) & west_to_east_diags[square] & ~SET_BIT(square);
                    break;
                case SE: // South East
                    ray_attacks[dir][square] = (full_board << (63 - square)) & west_to_east_diags[square] & ~SET_BIT(
                            square);
                    break;
                case SW: // South West
                    ray_attacks[dir][square] = (full_board << (63 - square)) & east_to_west_diags[square] & ~SET_BIT(
                            square);
                    break;
                default:
                    break;
            }
    }


    // Initialise knight_attacks

    for(square = 0; square < 64; square++) {
        Bitboard b = SET_BIT(square);
        int fileNo = square % 8; // The file position
        knight_attacks[square] = b >> 6 | b >> 10 | b >> 17 | b >> 15 | b << 6 | b << 10 | b << 17 | b << 15;

        if(fileNo == 6 || fileNo == 7) { // Stop wrap round the edges
            knight_attacks[square] &= ~(file[1] | file[0]);
        } else if(fileNo == 0 || fileNo == 1) {
            knight_attacks[square] &= ~(file[7] | file[6]);
        } // Bits just fall off of the end, so no need for rank
        // anti-wrapping
    }

    char *file_names[] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    char *rank_names[] = {"1", "2", "3", "4", "5", "6", "7", "8"};

    for(square = 0; square < 64; square++) {
        sqs[square] = malloc(3 * sizeof(char));
        strcpy(sqs[square], file_names[square % 8]);
        strcat(sqs[square], rank_names[square / 8]);
    }


    for (int sq = 0; sq < 64; sq++) {
        int file = sq % 8;
        Bitboard w_span = 0;
        Bitboard b_span = 0;

        if (file - 1 > 0) {
            w_span |= ray_attacks[N][sq - 1];
            b_span |= ray_attacks[S][sq - 1];
        }

        w_span |= ray_attacks[N][sq];
        b_span |= ray_attacks[S][sq];

        if (file + 1 < 8) {
            w_span |= ray_attacks[N][sq + 1];
            b_span |= ray_attacks[S][sq + 1];
        }

        front_attack_spans[WHITE][sq] = w_span;
        front_attack_spans[BLACK][sq] = b_span;
    }
}


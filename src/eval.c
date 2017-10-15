#include <stdio.h>
#include "board.h"
#include "eval.h"
#include "search.h"
#include "main.h"

int pawn_val = 100;
int knight_val = 320;
int bishop_val = 330;
int rook_val = 500;
int queen_val = 900;

int pawn_square_table[64] = {0,  0,  0,  0,  0,  0,  0,  0,
                             5, 10, 10,-20,-20, 10, 10,  5,
                             5, -5,-10,  0,  0,-10, -5,  5,
                             0,  0,  0, 20, 20,  0,  0,  0,
                             5,  5, 10, 25, 25, 10,  5,  5,
                             10, 10, 20, 30, 30, 20, 10, 10,
                             50, 50, 50, 50, 50, 50, 50, 50,
                             0,  0,  0,  0,  0,  0,  0,  0};

int knight_square_table[64] = {-50,-40,-30,-30,-30,-30,-40,-50,
                               -40,-20,  0,  5,  5,  0,-20,-40,
                               -30,  5, 10, 15, 15, 10,  5,-30,
                               -30,  0, 15, 20, 20, 15,  0,-30,
                               -30,  5, 15, 20, 20, 15,  5,-30,
                               -30,  0, 10, 15, 15, 10,  0,-30,
                               -40,-20,  0,  0,  0,  0,-20,-40,
                               -50,-40,-30,-30,-30,-30,-40,-50};

int bishop_square_table[64] = {-20,-10,-10,-10,-10,-10,-10,-20,
                                -10,  5,  0,  0,  0,  0,  5,-10,
                                -10, 10, 10, 10, 10, 10, 10,-10,
                                -10,  0, 10, 10, 10, 10,  0,-10,
                                -10,  5,  5, 10, 10,  5,  5,-10,
                                -10,  0,  5, 10, 10,  5,  0,-10,
                                -10,  0,  0,  0,  0,  0,  0,-10,
                                -20,-10,-10,-10,-10,-10,-10,-20};

int rook_square_table[64] = {0,  0,  0,  5,  5,  0,  0,  0,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             5, 10, 10, 10, 10, 10, 10,  5,
                             0,  0,  0,  0,  0,  0,  0,  0};

int queen_square_table[64] = {-20,-10,-10, -5, -5,-10,-10,-20,
                              -10,  0,  0,  0,  0,  0,  0,-10,
                              -10,  0,  5,  5,  5,  5,  0,-10,
                              -5,  0,  5,  5,  5,  5,  0, -5,
                               0,  0,  5,  5,  5,  5,  0, -5,
                              -10,  0,  5,  5,  5,  5,  0,-10,
                              -10,  0,  0,  0,  0,  0,  0,-10,
                              -20,-10,-10, -5, -5,-10,-10,-20};

int king_square_table[64] = {20, 30, 10,  0,  0, 10, 30, 20,
                             20, 20,  0,  0,  0,  0, 20, 20,
                             -10,-20,-20,-20,-20,-20,-20,-10,
                             -20,-30,-30,-40,-40,-30,-30,-20,
                             -30,-40,-40,-50,-50,-40,-40,-30,
                             -30,-40,-40,-50,-50,-40,-40,-30,
                             -30,-40,-40,-50,-50,-40,-40,-30,
                             -30,-40,-40,-50,-50,-40,-40,-30};

int* piece_tables[6] = {pawn_square_table, knight_square_table, bishop_square_table, rook_square_table, queen_square_table, king_square_table};

double basic_score(Board b) {
    double white_score = b->pop_count[PAWN] + 3 * (b->pop_count[KNIGHT] + b->pop_count[BISHOP]) +
                         5 * b->pop_count[ROOK] + 9 * b->pop_count[QUEEN];

    double black_score = b->pop_count[PAWN + 6] + 3 * (b->pop_count[KNIGHT + 6] + b->pop_count[BISHOP + 6]) +
                         5 * b->pop_count[ROOK + 6] + 9 * b->pop_count[QUEEN + 6];

    double result =  (b->side_to_move == WHITE ? 1 : -1) * (white_score - black_score);

    return result;
}

int pawn_eval(Board b, int side) {
    /* Evaluates a pawn structure for the given side based on:
     * 1. Double and triple pawns
     * 2. Passed pawns
     * 3. Isolated pawn
     * 4. Pawn chains
     * TODO: Add bonus for support for pawns guarding king queenside
     * */


    int double_pawn = -35;
    int passed_pawn = 40;
    int isolated_pawn[64] = {0, 0, 0, 0, 0, 0, 0, 0,
                             -20, -20, -20, -20, -20, -20, -20, -20,
                             -20, -20, -25, -25, -25, -25, -20, -20,
                             -20, -25, -25, -25, -25, -25, -25, -20,
                             -20, -25, -25, -25, -25, -25, -25, -20,
                             -20, -25, -25, -25, -25, -25, -25, -20,
                             -20, -20, -20, -20, -20, -20, -20, -20,
                              0, 0, 0, 0, 0, 0, 0, 0};

    int defender = 20;

    Bitboard pawn_board = b->pieces[PAWN + 6 * side];
    int score = 0;

    for (int f = 0; f < 8; f++) {
        int n = __builtin_popcount(file[f] & pawn_board);
        if (n > 0) score += double_pawn * (n - 1);
    }

    while (pawn_board) {
        int sq = pop_lsb(&pawn_board);
        Bitboard sq_board = SET_BIT(sq);

        // Passed pawn
        if (sq_board & front_attack_spans[side][sq]) score += passed_pawn;

        // Isolated pawn
        int f = sq % 8;
        Bitboard adjacent_files = 0;

        if (f > 0) adjacent_files |= file[f - 1];
        if (f < 8) adjacent_files |= file[f + 1];

        if (!(adjacent_files & (b->pieces[PAWN + 6 * side]))) score += isolated_pawn[sq];

        int pop_count = b->pop_count[13] + b->pop_count[14];

        if (side == WHITE) {
            if (NW_ONE(sq_board) && b->pieces[PAWN + 6 * side]) score += defender / (33 - pop_count);
            if (NE_ONE(sq_board) && b->pieces[PAWN + 6 * side]) score += defender / (33 - pop_count);
        } else {
            if (SW_ONE(sq_board) && b->pieces[PAWN + 6 * side]) score += defender / (33 - pop_count);
            if (SE_ONE(sq_board) && b->pieces[PAWN + 6 * side]) score += defender / (33 - pop_count);
        }

    }

    int line_of_three_bonus = 40;
    int edge_king_defence_bonus = 30;
    int centre_king_defence_bonus = 25;

    if (b->have_castled[0] && side == WHITE) {
        pawn_board = b->pieces[PAWN];
        Bitboard line_of_three = SET_BIT(f2) | SET_BIT(g2) | SET_BIT(h2);
        Bitboard h3_one_forward = SET_BIT(f2) | SET_BIT(g2) | SET_BIT(h3);
        Bitboard g3_one_forward = SET_BIT(f2) | SET_BIT(g3) | SET_BIT(h2);

        if ((pawn_board & line_of_three) == line_of_three) score += line_of_three_bonus;
        else if ((pawn_board & h3_one_forward) == h3_one_forward) score += edge_king_defence_bonus;
        else if ((pawn_board & g3_one_forward) == g3_one_forward) score += centre_king_defence_bonus;
    } else if (b->have_castled[2] && side == BLACK) {
        pawn_board = b->pieces[PAWN + 6];
        Bitboard line_of_three = SET_BIT(f7) | SET_BIT(g7) | SET_BIT(h7);
        Bitboard h6_one_forward = SET_BIT(f7) | SET_BIT(g7) | SET_BIT(h6);
        Bitboard g6_one_forward = SET_BIT(f7) | SET_BIT(g6) | SET_BIT(h7);

        if ((pawn_board & line_of_three) == line_of_three) score += line_of_three_bonus;
        else if ((pawn_board & h6_one_forward) == h6_one_forward) score += edge_king_defence_bonus;
        else if ((pawn_board & g6_one_forward) == g6_one_forward) score += centre_king_defence_bonus;
    }

    return score;
}


double eval(Board b, int n_moves) {
    int material_score = pawn_val * (b->pop_count[PAWN] - b->pop_count[PAWN + 6]) +
                         bishop_val * (b->pop_count[BISHOP] - b->pop_count[BISHOP + 6]) +
                         knight_val * (b->pop_count[KNIGHT] - b->pop_count[KNIGHT + 6]) +
                         rook_val * (b->pop_count[ROOK] - b->pop_count[ROOK + 6]) +
                         queen_val * (b->pop_count[QUEEN] - b->pop_count[QUEEN + 6]);

    int position_score = 0;

    for(int square = 0; square < 64; square++) {
        if (b->superbs[square] == BLACK && b->superbp[square]) position_score -= piece_tables[b->superbp[square] - 1][63 - square];
        else if (b->superbp[square]) position_score += piece_tables[b->superbp[square] - 1][square];
    }

    int gen_score = 0;

    int can_castle_king_side = 30;
    int can_castle_queen_side = 20;
    int castled_king_side = 95;
    int castled_queen_side = 70;

    int pop_count = b->pop_count[13] + b->pop_count[14];

    if (b->castling[0]) gen_score += can_castle_king_side;
    if (b->castling[1]) gen_score += can_castle_queen_side;
    if (b->castling[2]) gen_score -= can_castle_king_side;
    if (b->castling[3]) gen_score -= can_castle_queen_side;

    if (b->have_castled[0]) gen_score += castled_king_side / (33 - pop_count);
    if (b->have_castled[1]) gen_score += castled_queen_side / (33 - pop_count);
    if (b->have_castled[2]) gen_score -= castled_king_side / (33 - pop_count);
    if (b->have_castled[3]) gen_score -= castled_queen_side / (33 - pop_count);


    int pawn_score;
    uint64_t hash = pawn_hash(b);
    if (pt[hash % PAWN_TABLE_SIZE]->full_hash == hash) {
        n_pawn_hits++;
        pawn_score = pt[hash % PAWN_TABLE_SIZE]->val;
    } else {
        pawn_score = pawn_eval(b, WHITE) - pawn_eval(b, BLACK);
        pt_entry(b, pawn_score);
    }

    n_pawn_total++;

    int result =  (b->side_to_move == WHITE ? 1 : -1) * (material_score + position_score + pawn_score + gen_score + n_moves);
    return result;


}
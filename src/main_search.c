#include "search.h"
#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double pvs(Board b, int depth, double alpha, double beta, time_t start_time, int max_time);

Move find_move(Board b, int max_time) {
	time_t start_time = time(NULL);
	int depth = 0;
	Move best_move = 0;

	while(difftime(time(NULL), start_time) < max_time) {
		double score = pvs(b, ++depth, -INF, INF, start_time, max_time);

        if (difftime(time(NULL), start_time) < max_time) best_move = tt[zobrist_hash(b) % MAX_TABLE_SIZE]->best_move;
	    if(fabs(score) >= CHECKMATE - 1000) {
            return best_move;
        }
	}

	return best_move;
}


double pvs(Board b, int depth, double alpha, double  beta, time_t start_time, int max_time) {

    /* Principle variation search, aka negascout.
     * Uses alpha-beta pruning to remove known sub-optimal branches.
     * We do not need the exact score for all of the nodes, only the node path that will be optimal for both players
     * So once a path is found, we run a null window search to attempt to find a better move. If this fails,
     * we know there is a better path somewhere, so we re-run the search
     * */

	// TODO: - When in check, only generate moves that could possibly get us out of check
    // TODO: - Is it necessary to have these checks? Just set a really high value for the king???

    if(difftime(time(NULL), start_time) >= max_time) {
        return 0;
    }

    int curr_side = b->side_to_move;

    Bitboard attack_board = in_check(curr_side, b);

    Move killer_move, best_move;
    killer_move = best_move = 0;
    double null_window = beta;

    int can_move = 0 ;
    if (depth == 0 && !attack_board)  {
        return quiescient_search(alpha, beta, b, curr_side, start_time, max_time, depth);
    }

    Move* unordered_moves = malloc(sizeof(Move) * 512);
    int move_no = move_generation(b, (unsigned) curr_side, unordered_moves);
    Move* moves = move_sort(unordered_moves, b, move_no);
    free(unordered_moves);

    int start = moves[0] == 0 ? 1 : 0;

    for (int i = start; i < move_no + start; i++) {
        take_move(moves[i], b);

        if (!in_check(curr_side, b)) {
            can_move = 1;
            if (depth != 0) {
                double score = -pvs(b, depth - 1, -null_window, -alpha, start_time, max_time);

                if(score > alpha && score < beta && i > start) { // Re-search
                    score = -pvs(b, depth - 1, -beta, -alpha, start_time, max_time);
                }

                if (score > alpha) {
                    best_move = moves[i];
                    alpha = score;
                }

                if (alpha >= beta) {
                    killer_move = moves[i];
                    untake_move(moves[i], b);
                    break;
                }
                null_window = alpha + 1;
            }
        }

        untake_move(moves[i], b);
    }

    free(moves);

    tt_entry(b, killer_move, best_move, alpha);

    if (!can_move && attack_board) {
        return -INF;
    }  // Checkmate

    // We are in check, but not checkmate, so evaluate board normally
    if (depth == 0) return quiescient_search(alpha, beta, b, curr_side, start_time, max_time, depth);

    return alpha;
}


Move* move_sort(Move *moves, Board b, int move_no) {

    /* Sorts the moves in the following order:
     * - The best move that was calculated at the previous depth
     * - Any captures TODO: In order of value
     * - Then the remainder of the moves
     * */

    Move *ordered_moves = malloc((move_no + 1) * sizeof(Move));
	Move non_captures[move_no];


	int ordered_moves_index = 1;
	uint64_t hash = zobrist_hash(b);
	int key =  (int) (hash % MAX_TABLE_SIZE);
	Move hash_move = 0;

    if (hash == tt[key]->full_hash) n_tt_hits++;

    ordered_moves[0] = 0;

    /* ordered_moves[0] is for the hash move. But we can't add it until we know if it is pseudolegal due to hash collisions.
     * We do this by ensuring that when we process the rest of the moves, the hash move turns up there. If it remains
     * 0, it will be skipped in pvs above.
     * */

	// Get captures
	int non_captures_index = 0 ;

    for(int i = 0; i < move_no; i++) {
        if (moves[i] == hash_move) {
            n_tt_hits++;
            ordered_moves[0] = hash_move;
            continue;
        }

        if(get_captured(moves[i])) ordered_moves[ordered_moves_index++] = moves[i];
        else non_captures[non_captures_index++] = moves[i];
	}

	// Followed by the remainder
	for(int i = 0; i < non_captures_index; i++) ordered_moves[ordered_moves_index++] = non_captures[i];

    n_tt_total++;

	return ordered_moves;
}
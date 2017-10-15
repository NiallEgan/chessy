#include "board.h"
#include "move.h"
#include "search.h"
#include "eval.h"
#include "main.h"

#include <stdlib.h>
#include <stdio.h>

#define POPULATE(pt) do { \
				Move *p_record = (pt); \
				while(*(pt)) moves[n_captures++] = *(pt)++; \
				free(p_record); \
				} while(0)


double quiescient_search(double alpha, double beta, Board b, int side, time_t max_time, time_t start_time, int depth) {
    // Runs an alpha-beta search with only certain moves considered (namely captures) to stop the horizon effect

    // TODO: Add checks and promotions to quescience

    double score = eval(b, 0);

    if(score >= beta) return beta;  // alpha-beta pruning

    if(alpha < score) alpha = score;

    Move *moves = malloc(sizeof(Move) * 256);

    int move_no = move_generation(b, (unsigned) side, moves);
    int n_captures = quiescient_ordering(moves, (unsigned) move_no);


    for(int i = 0; i < n_captures; i++) {
        //if(time(NULL) - start_time > max_time) break;

        take_move(moves[i], b);
        score  = -quiescient_search(-beta, -alpha, b, !side, max_time, start_time, depth + 1);
        untake_move(moves[i], b);

        if(score >= beta) {
            free(moves);
            return beta;
        }

        if(score > alpha) alpha = score;
    }

    free(moves);
    return alpha;
}


int quiescient_ordering(Move *moves, size_t move_no) {
    // Orders the captures in order of points gain, filters out the remainder of the moves

    // axb means piece a takes piece b
    Move *pxq = calloc(move_no,  sizeof(Move)); // Only consider non-negative captures
    Move *nxq = calloc(move_no,  sizeof(Move));
    Move *bxq = calloc(move_no,  sizeof(Move));
    Move *rxq = calloc(move_no,  sizeof(Move));
    Move *qxq = calloc(move_no,  sizeof(Move));
    Move *pxr = calloc(move_no,  sizeof(Move));
    Move *nxr = calloc(move_no,  sizeof(Move));
    Move *bxr = calloc(move_no,  sizeof(Move));
    Move *rxr = calloc(move_no,  sizeof(Move));
    Move *pxb = calloc(move_no,  sizeof(Move));
    Move *nxb = calloc(move_no,  sizeof(Move));
    Move *bxb = calloc(move_no,  sizeof(Move));
    Move *pxn = calloc(move_no,  sizeof(Move));
    Move *nxn = calloc(move_no,  sizeof(Move));
    Move *bxn = calloc(move_no,  sizeof(Move));
    Move *pxp = calloc(move_no,  sizeof(Move));

    int pxq_n = 0;
    int nxq_n = 0;
    int bxq_n = 0;
    int rxq_n = 0;
    int qxq_n = 0;
    int pxr_n = 0;
    int nxr_n = 0;
    int bxr_n = 0;
    int rxr_n = 0;
    int pxb_n = 0;
    int nxb_n = 0;
    int bxb_n = 0;
    int pxn_n = 0;
    int nxn_n = 0;
    int bxn_n = 0;
    int pxp_n = 0;

    for(int i = 0; i < move_no; i++) {
        int captured_piece = get_captured(moves[i]);
        int moving_piece = get_piece(moves[i]);

        switch (captured_piece)  {
            case PAWN:
                if(moving_piece == PAWN)
                    pxp[pxp_n++] = moves[i];
                break;

            case KNIGHT:
                switch(moving_piece) {
                    case PAWN:
                        pxn[pxn_n++] = moves[i];
                        break;

                    case KNIGHT:
                        nxn[nxn_n++] = moves[i];
                        break;

                    case BISHOP:
                        bxn[bxn_n++] = moves[i];
                        break;

                    default:
                        break;
                } break;

            case BISHOP:
                switch(moving_piece) {
                    case PAWN:
                        pxb[pxb_n++] = moves[i];
                        break;

                    case KNIGHT:
                        nxb[nxb_n++] = moves[i];
                        break;

                    case BISHOP:
                        bxb[bxb_n++] = moves[i];
                        break;

                    default:
                        break;
                } break;

            case ROOK:
                switch(moving_piece) {
                    case PAWN:
                        pxr[pxr_n++] = moves[i];
                        break;

                    case KNIGHT:
                        nxr[nxr_n++] = moves[i];
                        break;

                    case BISHOP:
                        bxr[bxr_n++] = moves[i];
                        break;

                    case ROOK:
                        rxr[rxr_n++] = moves[i];
                        break;

                    default:
                        break;
                } break;

            case QUEEN:
                switch(moving_piece) {
                    case PAWN:
                        pxq[pxq_n++] = moves[i];
                        break;

                    case KNIGHT:
                        nxq[nxq_n++] = moves[i];
                        break;

                    case BISHOP:
                        bxq[bxq_n++] = moves[i];
                        break;

                    case ROOK:
                        rxq[rxq_n++] = moves[i];
                        break;

                    case QUEEN:
                        qxq[qxq_n++] = moves[i];
                        break;

                    default:
                        break;
                } break;

            default:
                break;
        }
    }

    int n_captures = 0;

    POPULATE(pxq);
    POPULATE(nxq);
    POPULATE(bxq);
    POPULATE(rxq);
    POPULATE(qxq);
    POPULATE(pxr);
    POPULATE(nxr);
    POPULATE(bxr);
    POPULATE(rxr);
    POPULATE(pxn);
    POPULATE(nxn);
    POPULATE(bxn);
    POPULATE(pxb);
    POPULATE(nxb);
    POPULATE(bxb);
    POPULATE(pxp);

    return n_captures;
}
#include "search.h"

#include <intrin.h>
#include <cmath>

#include "data.h"
#include "movegen.h"


float negamaxHeuristic(State state, MoveLookup *lookups)
{
    int pieceDiff = _mm_popcnt_u32(state.allPieces[0] & boardMask) - _mm_popcnt_u32(state.allPieces[1] & boardMask);
    int centerDiff = _mm_popcnt_u32(state.allPieces[0] & centerSquares) - _mm_popcnt_u32(state.allPieces[1] & centerSquares);
    Bitboard blueKing = state.allPieces[0] & state.kings;
    Bitboard redKing = state.allPieces[1] & state.kings;
    int kingDiff = _mm_popcnt_u32(blueKing) - _mm_popcnt_u32(redKing);
    int kingStartingDiff = _mm_popcnt_u32(blueKing & redStartingSquare) - _mm_popcnt_u32(redKing & blueStartingSquare);
    Bitboard allBlueMoves = allMovesForBoard(state.allPieces[0], lookups, 0);
    Bitboard allRedMoves = allMovesForBoard(state.allPieces[1], lookups, 1);
    int coverageDiff = _mm_popcnt_u32(allBlueMoves) - _mm_popcnt_u32(allRedMoves);
    int kingInCheckDiff = ((redKing & allBlueMoves) != 0) - ((blueKing & allRedMoves) != 0);

    // @formatter:off
    return
        pieceDiff        * 1 +
        kingInCheckDiff  * 0.8 +
        centerDiff       * 0.2 +
        coverageDiff     * 0.07 +
        kingDiff         * 100 +
        kingStartingDiff * 100;
    // @formatter:on
}

// Negamax with alpha-beta pruning. color is 0 or 1.
SearchValue negamaxWithAbPruning(State state, MoveLookup *lookups, float alpha, float beta, int depth, int color, bool start)
{
    if (depth == 0 || checkWinCondition(state) != -1) {
        int colorMultiplier = color == 0 ? 1 : -1;
        float evaluation = (float) negamaxHeuristic(state, lookups) * colorMultiplier;
        return {state, evaluation};
    }

    StateArray states;
    int stateSize = nextStatesForBoard(state, lookups, color, states);

    float bestValue = -INFINITY;
    State bestState;

    for (int i = 0; i < stateSize; ++i) {
        SearchValue nextDepthNegamax = negamaxWithAbPruning(states[i], lookups, -beta, -alpha, depth - 1, 1 - color, false);

        if (bestValue < -nextDepthNegamax.value) {
            bestValue = -nextDepthNegamax.value;
            bestState = nextDepthNegamax.state;
        }

        alpha = std::max(alpha, bestValue);

        if (alpha >= beta)
            break;
    }

    if (start == 0)
        bestState = state;

    return {bestState, bestValue};
}

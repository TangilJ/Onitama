#include <intrin.h>
#include <cmath>
#include "search.h"
#include "movegen.h"

float negamaxHeuristic(State state)
{
    int pieceDiff = _mm_popcnt_u32(state.allPieces[0] & boardMask) - _mm_popcnt_u32(state.allPieces[1] & boardMask);
    int centerDiff = _mm_popcnt_u32(state.allPieces[0] & centerSquares) - _mm_popcnt_u32(state.allPieces[1] & centerSquares);
    int kingDiff = _mm_popcnt_u32(state.allPieces[0] & state.kings) - _mm_popcnt_u32(state.allPieces[1] & state.kings);
    int kingStartingDiff = _mm_popcnt_u32(state.allPieces[0] & state.kings & redStartingSquare) -
                           _mm_popcnt_u32(state.allPieces[1] & state.kings & blueStartingSquare);

    return pieceDiff + centerDiff * 0.5 + kingDiff * 100 + kingStartingDiff * 100;
}

// Negamax without alpha-beta pruning. color is 0 or 1.
SearchValue negamax(State state, MoveLookup *lookups, int depth, int color, bool start)
{
    if (depth == 0 || checkWinCondition(state)) {
        float heuristicValue = negamaxHeuristic(state);
        SearchValue searchValue = {state, heuristicValue};
        return searchValue;
    }

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, color, array);

    float bestValue = -INFINITY;
    State bestState;
    for (int i = 0; i < stateSize; ++i) {
        SearchValue nextDepthNegamax = negamax(array[i], lookups, depth - 1, 1 - color, false);
        if (bestValue < -nextDepthNegamax.value) {
            bestValue = -nextDepthNegamax.value;
            bestState = nextDepthNegamax.state;
        }
    }

    if (start == 0)
        bestState = state;

    return {bestState, bestValue};
}


// Negamax with alpha-beta pruning. color is 0 or 1.
SearchValue negamaxWithAbPruning(State state, MoveLookup *lookups, float alpha, float beta, int depth, int color, bool start)
{
    if (depth == 0 || checkWinCondition(state)) {
        float heuristicValue = (float) negamaxHeuristic(state);
        SearchValue searchValue = {state, heuristicValue};
        return searchValue;
    }

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, color, array);

    float bestValue = -INFINITY;
    State bestState;

    for (int i = 0; i < stateSize; ++i) {
        SearchValue nextDepthNegamax = negamaxWithAbPruning(array[i], lookups, -beta, -alpha, depth - 1, 1 - color, false);

        if (bestValue < -nextDepthNegamax.value) {
            bestValue = -nextDepthNegamax.value;
            bestState = nextDepthNegamax.state;
        }

        alpha = alpha > bestValue ? alpha : bestValue;

        if (alpha >= beta)
            break;
    }

    if (start == 0)
        bestState = state;

    SearchValue searchValue = {bestState, bestValue};
    return searchValue;
}
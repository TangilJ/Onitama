#include <x86intrin.h>
#include <cmath>
#include "search.h"
#include "movegen.h"

int negamaxHeuristic(State state)
{
    int pieceDiff = _mm_popcnt_u32(state.allPieces[0] & boardMask) - _mm_popcnt_u32(state.allPieces[1] & boardMask);
    int centerDiff = _mm_popcnt_u32(state.allPieces[0] & centerSquares) - _mm_popcnt_u32(state.allPieces[1] & centerSquares);
    return pieceDiff + centerDiff;
}

// Negamax without alpha-beta pruning. color is 1 or -1.
SearchValue Negamax(State state, MoveLookup lookups[], int depth, int color, bool start)
{
    if (depth == 0) {
        float heuristicValue = (float) negamaxHeuristic(state);
        SearchValue searchValue = {state, heuristicValue};
        return searchValue;
    }

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, color, array);

    float bestValue = -INFINITY;
    State bestState;
    for (int i = 0; i < stateSize; ++i) {
        SearchValue nextDepthNegamax = Negamax(array[i], lookups, depth - 1, 1 - color, false);
        if (bestValue < -nextDepthNegamax.value) {
            bestValue = -nextDepthNegamax.value;
            bestState = nextDepthNegamax.state;
        }
    }

    if (start == 0)
        bestState = state;

    return {bestState, bestValue};
}

#include <intrin.h>
#include <cmath>
#include "search.h"
#include "movegen.h"
#include "zobrist.h"

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
        float heuristicValue = (float) negamaxHeuristic(state, lookups) * colorMultiplier;
        return {state, heuristicValue};
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

        alpha = std::max(alpha, bestValue);

        if (alpha >= beta)
            break;
    }

    if (start == 0)
        bestState = state;

    return {bestState, bestValue};
}

// Negamax with alpha-beta pruning and a transposition table. color is 0 or 1.
SearchValue negamaxABAndTT(State state, MoveLookup *lookups, float alpha, float beta, int depth, int color, TTEntry *tTable,
                           ZobristTable &zTable, int tTableSize, bool start)
{
    float originalAlpha = alpha;

    ZobristKey key = hashZobrist(state, zTable);
    TTEntry *entry = &tTable[key % tTableSize];
    if (entry->state == state && entry->depth >= depth) {
        if (entry->flag == TTFlag::Exact)
            return {state, entry->value};
        if (entry->flag == TTFlag::LowerBound)
            alpha = std::max(alpha, entry->value);
        else if (entry->flag == TTFlag::UpperBound)
            beta = std::min(beta, entry->value);

        if (alpha >= beta)
            return {state, entry->value};
    }

    if (depth == 0 || checkWinCondition(state) != -1) {
        int colorMultiplier = color == 0 ? 1 : -1;
        float heuristicValue = (float) negamaxHeuristic(state, lookups) * colorMultiplier;
        return {state, heuristicValue};
    }

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, color, array);

    float bestValue = -INFINITY;
    State bestState;

    for (int i = 0; i < stateSize; ++i) {
        SearchValue nextDepthNegamax = negamaxABAndTT(array[i], lookups, -beta, -alpha, depth - 1, 1 - color, tTable, zTable,
                                                      tTableSize, false);

        if (bestValue < -nextDepthNegamax.value) {
            bestValue = -nextDepthNegamax.value;
            bestState = nextDepthNegamax.state;
        }

        alpha = alpha > bestValue ? alpha : bestValue;

        if (alpha >= beta)
            break;
    }

    entry->value = bestValue;
    if (bestValue <= originalAlpha)
        entry->flag = TTFlag::UpperBound;
    else if (bestValue >= beta)
        entry->flag = TTFlag::LowerBound;
    else
        entry->flag = TTFlag::Exact;
    entry->depth = depth;
    entry->state = state;

    if (start == 0)
        bestState = state;

    return {bestState, bestValue};
}
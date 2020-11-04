#include <x86intrin.h>
#include "movegen.h"

Bitboard allMovesForPiece(Bitboard board, MoveLookup lookup, int playerIndex, int pieceIndex)
{
    return lookup[playerIndex][pieceIndex] & ~board;
}

Bitboard allMovesForPiece(Bitboard piece, Bitboard board, MoveLookup lookup, int playerIndex)
{
    int index = 31 - _bit_scan_forward(piece);
    return allMovesForPiece(board, lookup, playerIndex, index);
}

std::tuple<StateArray, int> nextStatesForBoardMonstrosity(State state, MoveLookup lookups[], int playerIndex)
{
    StateArray nextStates;
    int nextStateSize = 0;
    Bitboard boardAndCards = state.allPieces[playerIndex];
    Bitboard boardOnly = boardAndCards & boardMask;

    // Get all the next states for the entire board for both cards
    Bitboard boardPiecesToGo = boardOnly;
    while (boardPiecesToGo != 0) {
        Bitboard boardSquare = boardPiecesToGo & -boardPiecesToGo;
        boardPiecesToGo ^= boardSquare;

        // Get all the next states for this piece for both cards
        if (boardSquare & boardAndCards) {
            Bitboard lastCard = 0;
            for (int cardNum = 0; cardNum < 2; ++cardNum) {
                int cardBitPosition = _bit_scan_forward(boardAndCards ^ lastCard);

                Bitboard cardBit = 1 << cardBitPosition;
                // Get all the next states for this piece for the current card
                Bitboard moveMask = allMovesForPiece(boardSquare, boardAndCards & boardMask,
                                                     lookups[cardBitPosition], playerIndex);
                Bitboard newCards = boardAndCards & ~boardMask ^ cardBit | (state.kings & ~boardMask);

                Bitboard moveMaskPiecesToGo = moveMask;
                while (moveMaskPiecesToGo != 0) {
                    Bitboard currentSquare = moveMaskPiecesToGo & -moveMaskPiecesToGo;
                    moveMaskPiecesToGo ^= currentSquare;
                    if (currentSquare & moveMask) {
                        Bitboard newPieces = boardOnly ^ boardSquare | currentSquare | newCards;
                        nextStates[nextStateSize].allPieces[playerIndex] = newPieces;
                        nextStates[nextStateSize].allPieces[1 - playerIndex] = state.allPieces[1 - playerIndex];
                        nextStates[nextStateSize].kings = state.kings & boardMask | cardBit;
                        nextStateSize += 1;
                    }
                }
                lastCard = cardBit;
            }
        }
    }
    return {nextStates, nextStateSize};
}

// 0 = p0 win, 1 = p1 win, -1 = game has not ended
int checkWinCondition(State state)
{
    Bitboard kings = state.kings & boardMask;
    if (_mm_popcnt_u32(kings) != 2)
        return _mm_popcnt_u32(kings & state.allPieces[1]);
    if (state.allPieces[0] & 0b00000000000000000000001000000000)
        return 0;
    if (state.allPieces[1] & 0b00100000000000000000000000000000)
        return 1;
    return -1;
}

#ifndef ONITAMA_MOVEGEN_H
#define ONITAMA_MOVEGEN_H

#include "main.h"

Bitboard allMovesForPiece(Bitboard board, MoveLookup lookup, int playerIndex, int pieceIndex);

Bitboard allMovesForPiece(Bitboard piece, Bitboard board, MoveLookup lookup, int playerIndex);

std::tuple<StateArray, int> nextStatesForBoardMonstrosity(State state, MoveLookup lookups[], int playerIndex);

int checkWinCondition(State state);

#endif //ONITAMA_MOVEGEN_H

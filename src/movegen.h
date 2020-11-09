#ifndef ONITAMA_MOVEGEN_H
#define ONITAMA_MOVEGEN_H

#include "main.h"

Bitboard allMovesForPiece(Bitboard board, MoveLookup &lookup, int playerIndex, int pieceIndex);

Bitboard allMovesForPiece(Bitboard piece, Bitboard board, MoveLookup &lookup, int playerIndex);

Bitboard allMovesForBoard(Bitboard board, MoveLookup lookup, int playerIndex);

int nextStatesForBoard(State state, MoveLookup *lookups, int playerIndex, StateArray &nextStates);

int checkWinCondition(State state);

#endif //ONITAMA_MOVEGEN_H

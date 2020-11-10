#ifndef ONITAMA_PERFT_H
#define ONITAMA_PERFT_H

#include "main.h"

unsigned long long perft(State state, int depth, int playerIndex, bool start);

void printPerftSpeed(State state, int depth, int playerIndex);

void printIncreasingPerftSpeed(State state, int depth, int playerIndex);

#endif
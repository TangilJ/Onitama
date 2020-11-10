#ifndef ONITAMA_UTILITIES_H
#define ONITAMA_UTILITIES_H

#include "main.h"

void printBits(Bitboard board);

void printBoard(State state);

void printBoard(Bitboard board);

std::array<MoveLookup, 5> getLookupsFromNames(std::vector<std::string> names);

#endif //ONITAMA_UTILITIES_H

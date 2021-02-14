#ifndef ONITAMA_UTILITIES_H
#define ONITAMA_UTILITIES_H

#include <vector>
#include "types.h"

void printBits(Bitboard board);

void printBoard(State state);

void printBoard(Bitboard board);

void getLookupsFromNames(std::vector<std::string> names, MoveLookup *lookups);

void getRandomCards(MoveLookup *lookups);

#endif //ONITAMA_UTILITIES_H

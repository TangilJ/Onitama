#ifndef ONITAMA_UTILITIES_H
#define ONITAMA_UTILITIES_H

#include "main.h"

void printBits(Bitboard board);

void printBoard(State state);

void printBoard(Bitboard board);

void getLookupsFromNames(std::vector<std::string> names, MoveLookup *lookups);

void getRandomCards(MoveLookup *lookups);

void getStateFromServerString(const std::string &string, State &state);

std::string serverMoveStringFromStates(State first, State second, std::vector<std::string> names);

#endif //ONITAMA_UTILITIES_H

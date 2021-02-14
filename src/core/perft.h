#ifndef ONITAMA_PERFT_H
#define ONITAMA_PERFT_H

#include "types.h"

unsigned long long perft(State state, int depth, int playerIndex, MoveLookup *lookups);

unsigned long long parallelPerft(State state, int depth, int playerIndex, MoveLookup *lookups);

#endif //ONITAMA_PERFT_H

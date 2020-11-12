#ifndef ONITAMA_SEARCH_H
#define ONITAMA_SEARCH_H

#include "main.h"

typedef struct {
    State state;
    float value;
} SearchValue;

float negamaxHeuristic(State state, MoveLookup *lookups);

SearchValue negamax(State state, MoveLookup *lookups, int depth, int color, bool start);

SearchValue negamaxWithAbPruning(State state, MoveLookup *lookups, float alpha, float beta, int depth, int color, bool start);

#endif //ONITAMA_SEARCH_H

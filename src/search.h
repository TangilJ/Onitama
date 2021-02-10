#ifndef ONITAMA_SEARCH_H
#define ONITAMA_SEARCH_H

#include "main.h"

typedef struct {
    State state;
    float value;
} SearchValue;

float negamaxHeuristic(State state, MoveLookup *lookups);

SearchValue negamaxWithAbPruning(State state, MoveLookup *lookups, float alpha, float beta, int depth, int color, bool start);

#endif //ONITAMA_SEARCH_H

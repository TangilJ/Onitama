#ifndef ONITAMA_SEARCH_H
#define ONITAMA_SEARCH_H

#include "main.h"

typedef struct {
    State state;
    float value;
} SearchValue;

int negamaxHeuristic(State state);

SearchValue Negamax(State state, MoveLookup lookups[], int depth, int color, bool start);

#endif //ONITAMA_SEARCH_H

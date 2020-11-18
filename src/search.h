#ifndef ONITAMA_SEARCH_H
#define ONITAMA_SEARCH_H

#include "main.h"

typedef struct {
    State state;
    float value;
} SearchValue;

enum class TTFlag {
    Exact,
    LowerBound,
    UpperBound
};

struct TTEntry {
    bool valid;
    int depth;
    TTFlag flag;
    float value;
};

float negamaxHeuristic(State state, MoveLookup *lookups);

SearchValue negamaxWithAbPruning(State state, MoveLookup *lookups, float alpha, float beta, int depth, int color, bool start);

SearchValue negamaxABAndTT(State state, MoveLookup *lookups, float alpha, float beta, int depth, int color, TTEntry *tTable,
                           ZobristTable &zTable, int tTableSize, bool start);

#endif //ONITAMA_SEARCH_H

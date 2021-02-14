#ifndef ONITAMA_PERFTMODE_H
#define ONITAMA_PERFTMODE_H

#include <core/types.h>

#include "Mode.h"


struct PerftMode : Mode {
private:
    void printIncreasingPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel);

    void printPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel);

public:
    bool increasingPerft{false};
    bool parallelPerftOption{false};
    int startingPlayer{0};

    void run() override;
};


#endif //ONITAMA_PERFTMODE_H

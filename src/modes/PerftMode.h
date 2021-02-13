#ifndef ONITAMA_PERFTMODE_H
#define ONITAMA_PERFTMODE_H

// GCC doesn't support <execution> without relying on Intel TBB, so we'll just
// disable anything parallel if the compiler being used is GCC.
#ifndef __GNUC__
#include <execution>
#endif

#include <algorithm>
#include <ctime>
#include <string>
#include "../main.h"
#include "../data.h"
#include "../utilities.h"
#include "../movegen.h"
#include "Mode.h"

struct PerftMode : Mode {
private:
    unsigned long long perft(State state, int depth, int playerIndex, MoveLookup *lookups);

    unsigned long long parallelPerft(State state, int depth, int playerIndex, MoveLookup *lookups);

    void printIncreasingPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel);

    void printPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel);

public:
    bool increasingPerft{false};
    bool parallelPerftOption{false};

    void run() override;
};


#endif //ONITAMA_PERFTMODE_H
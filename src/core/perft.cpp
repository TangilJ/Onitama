#include "perft.h"

// GCC doesn't support <execution> without relying on Intel TBB, so we'll just
// disable anything parallel if the compiler being used is GCC.
#ifndef __GNUC__
#include <execution>
#endif

#include "movegen.h"


unsigned long long perft(State state, int depth, int playerIndex, MoveLookup *lookups)
{
    if (depth == 0 || checkWinCondition(state) != -1)
        return 1;

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, playerIndex, array);

    unsigned long long total = 0;
    for (int i = 0; i < stateSize; ++i) {
        total += perft(array[i], depth - 1, 1 - playerIndex, lookups);
    }

    return total;
}

unsigned long long parallelPerft(State state, int depth, int playerIndex, MoveLookup *lookups)
{
#ifndef __GNUC__
    if (depth == 0 || checkWinCondition(state) != -1)
        return 1;

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, playerIndex, array);

    unsigned long long total = 0;
    // TODO: Currently spawns a thread per state in the starting ply. Should instead spawn a certain number of max states.
    std::for_each(std::execution::par, array.begin(), array.begin() + stateSize, [&](State item) mutable {
        total += perft(item, depth - 1, 1 - playerIndex, lookups);
    });

    return total;
#else
    throw std::runtime_error("This executable was built using GCC which does not support parallel execution. "
                             "To run parallel perft, run a build which was built with Clang.");
#endif
}
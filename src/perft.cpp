#include <ctime>
#include <string>

// GCC doesn't support <execution> without relying on Intel TBB, so we'll just
// disable anything parallel if the compiler being used is GCC.
#ifndef __GNUC__
#include <execution>
#endif

#include "data.h"
#include "perft.h"
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

void printIncreasingPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel)
{
    for (int i = 1; i < depth + 1; ++i)
        printPerftSpeed(state, i, playerIndex, lookups, parallel);
}

void printPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel)
{
    std::clock_t start = std::clock();

    unsigned long long total;
    if (parallel)
        total = parallelPerft(state, depth, playerIndex, lookups);
    else
        total = perft(state, depth, playerIndex, lookups);

    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    double speed = (double) total / duration / 1000000;
    printf("Depth %i (%10llu nodes) took %.5ssec = (%.5s Mnodes/sec)\n", depth, total,
           std::to_string(duration).c_str(), std::to_string(speed).c_str());
}

#include <ctime>
#include <string>
#include <execution>
#include "data.h"
#include "perft.h"
#include "movegen.h"


unsigned long long perft(State state, int depth, int playerIndex, MoveLookup *lookups, bool start = false)
{
    if (depth == 0)
        return 1;
    if (checkWinCondition(state) != -1)
        return 1;

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, playerIndex, array);

    unsigned long long total = 0;

    // TODO: Currently spawns a thread per state in the starting ply. Should instead spawn a certain number of max states.
    if (start)
        std::for_each(std::execution::par, array.begin(), array.begin() + stateSize, [&](State item) mutable {
            total += perft(item, depth - 1, 1 - playerIndex, lookups);
        });
    else
        for (int i = 0; i < stateSize; ++i) {
            total += perft(array[i], depth - 1, 1 - playerIndex, lookups);
        }

    return total;
}

void printIncreasingPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups)
{
    for (int i = 1; i < depth + 1; ++i)
        printPerftSpeed(state, i, playerIndex, lookups);
}

void printPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups)
{
    std::clock_t start = std::clock();
    unsigned long long total = perft(state, depth, playerIndex, lookups, true);
    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    double speed = (double) total / duration / 1000000;
    printf("Depth %i (%10llu nodes) took %.5ssec = (%.5s Mnodes/sec)\n", depth, total,
           std::to_string(duration).c_str(), std::to_string(speed).c_str());
}

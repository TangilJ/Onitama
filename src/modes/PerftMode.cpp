#include "PerftMode.h"

void PerftMode::run()
{
    printf("Running ");
    if (this->increasingPerft)
        printf("increasing ");
    if (this->parallelPerftOption)
        printf("parallel ");
    printf("perft with depth %i\n\n", this->depth);

    if (std::find(this->cards.begin(), this->cards.end(), "") != this->cards.end())
        this->cards = {"ox", "boar", "horse", "elephant", "crab"};
    MoveLookup lookupsArray[5];
    getLookupsFromNames(this->cards, lookupsArray);

    printf(
        "Perft starting configuration:\n"
        "blue = %s, %s\n"
        "red = %s, %s\n"
        "side = %s\n\n",
        this->cards[0].c_str(), this->cards[1].c_str(),
        this->cards[2].c_str(), this->cards[3].c_str(),
        this->cards[4].c_str()
    );

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    if (this->increasingPerft)
        printIncreasingPerftSpeed(state, this->depth, 0, lookupsArray, this->parallelPerftOption);
    else
        printPerftSpeed(state, this->depth, 0, lookupsArray, this->parallelPerftOption);
}

unsigned long long PerftMode::perft(State state, int depth, int playerIndex, MoveLookup *lookups)
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

unsigned long long PerftMode::parallelPerft(State state, int depth, int playerIndex, MoveLookup *lookups)
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

void PerftMode::printIncreasingPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel)
{
    for (int i = 1; i < depth + 1; ++i)
        printPerftSpeed(state, i, playerIndex, lookups, parallel);
}

void PerftMode::printPerftSpeed(State state, int depth, int playerIndex, MoveLookup *lookups, bool parallel)
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

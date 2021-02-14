#include "PerftMode.h"

#include <algorithm>
#include <ctime>
#include <string>

#include <core/perft.h>
#include <core/data.h>
#include <core/utilities.h>


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
        "side = %s\n"
        "(%s starts)\n\n",
        this->cards[0].c_str(), this->cards[1].c_str(),
        this->cards[2].c_str(), this->cards[3].c_str(),
        this->cards[4].c_str(),
        this->startingPlayer == 0 ? "blue" : "red"
    );

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    if (this->increasingPerft)
        printIncreasingPerftSpeed(state, this->depth, this->startingPlayer, lookupsArray, this->parallelPerftOption);
    else
        printPerftSpeed(state, this->depth, this->startingPlayer, lookupsArray, this->parallelPerftOption);
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
    printf(
        "Depth %i: %10llu nodes in %6.4ss (%.5s Mnodes/s)\n",
        depth, total, std::to_string(duration).c_str(), std::to_string(speed).c_str()
    );
}

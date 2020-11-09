#include <ctime>
#include <string>
#include "data.h"
#include "perft.h"
#include "movegen.h"

MoveLookup lookups[5] = {oxMoves, boarMoves, horseMoves, elephantMoves, crabMoves};

unsigned long perft(State state, int depth, int playerIndex)
{
    if (depth == 0)
        return 1;
    if (checkWinCondition(state) != -1)
        return 1;

    StateArray array;
    int stateSize = nextStatesForBoard(state, lookups, playerIndex, array);

    int total = 0;

    for (int i = 0; i < stateSize; ++i) {
        total += perft(array[i], depth - 1, 1 - playerIndex);
    }

    return total;
}

void printIncreasingPerftSpeed(State state, int depth, int playerIndex)
{
    for (int i = 1; i < depth + 1; ++i)
        printPerftSpeed(state, i, playerIndex);
}

void printPerftSpeed(State state, int depth, int playerIndex)
{
    std::clock_t start = std::clock();
    unsigned long total = perft(state, depth, playerIndex);
    double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
    double speed = (double) total / duration / 1000000;
    printf("Depth %i (%10lu nodes) took %.5ssec = (%.5s Mnodes/sec)\n", depth, total,
           std::to_string(duration).c_str(), std::to_string(speed).c_str());
}

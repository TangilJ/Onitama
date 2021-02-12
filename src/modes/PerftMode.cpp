#include "PerftMode.h"

void PerftMode::run()
{
    printf("Running perft with depth %i\n\n", this->depth);

    if (std::find(this->cards.begin(), this->cards.end(), "") != this->cards.end())
        this->cards = {"ox", "boar", "horse", "elephant", "crab"};
    MoveLookup lookupsArray[5];
    getLookupsFromNames(this->cards, lookupsArray);

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    if (this->increasingPerft)
        printIncreasingPerftSpeed(state, this->depth, 0, lookupsArray, this->parallelPerft);
    else
        printPerftSpeed(state, this->depth, 0, lookupsArray, this->parallelPerft);
}

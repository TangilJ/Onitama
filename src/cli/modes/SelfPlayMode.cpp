#include "../../core/data.h"
#include "SelfPlayMode.h"


void SelfPlayMode::run()
{
    MoveLookup lookups[5];
    if (std::find(this->cards.begin(), this->cards.end(), "") != this->cards.end())
        getRandomCards(lookups);
    else
        getLookupsFromNames(this->cards, lookups);

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    printf("Running self-play with negamax with alpha-beta pruning on depth %i\n\n", this->depth);

    puts("Ply 0");
    printBoard(state);
    puts("");

    int color = 0;
    int ply = 0;
    while (checkWinCondition(state) == -1) {
        const SearchValue &negamax = negamaxWithAbPruning(state, lookups, -INFINITY, INFINITY, this->depth, color, true);
        state = negamax.state;
        color = 1 - color;
        ply++;

        printf("Ply %i\n", ply);
        printBoard(state);
        printf("Evaluation: %.2f\n\n", negamax.value);
    }
}
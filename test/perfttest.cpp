#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest/doctest.h>
#include "../src/modes/PerftMode.h"


// Perft values were collected from the Onitama community


void runPerftChecks(std::vector<std::string> cards, int startingPlayerIndex, std::vector<unsigned long long> perftValues)
{
    MoveLookup lookupsArray[5];
    getLookupsFromNames(cards, lookupsArray);

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    for (int i = 0; i < perftValues.size(); ++i) {
        CHECK(PerftMode::perft(state, i + 1, startingPlayerIndex, lookupsArray) == perftValues[i]);
    }
}


TEST_CASE("perft [blue = ox, boar] [red = horse, elephant] [side = crab] [blue starts]")
{
    std::vector<std::string> cards = {"ox", "boar", "horse", "elephant", "crab"};
    std::vector<unsigned long long> perftValues = {10, 130, 1989, 28509, 487780, 7748422, 137281607, 2353802670};
    runPerftChecks(cards, 0, perftValues);
}


TEST_CASE("perft [blue = rabbit, cobra] [red = rooster, tiger] [side = frog] [red starts]")
{
    std::vector<std::string> cards = {"rabbit", "cobra", "rooster", "tiger", "frog"};
    std::vector<unsigned long long> perftValues = {9, 72, 880, 10374, 138879, 1781181, 24489752, 324013378};
    runPerftChecks(cards, 1, perftValues);
}


TEST_CASE("perft [blue = goose, dragon] [red = mantis, eel] [side = crane] [blue starts]")
{
    std::vector<std::string> cards = {"goose", "dragon", "mantis", "eel", "crane"};
    std::vector<unsigned long long> perftValues = {10, 120, 1272, 16445, 211643, 2793554, 39359208, 537916817};
    runPerftChecks(cards, 0, perftValues);
}


TEST_CASE("perft [blue = monkey, tiger] [red = crab, dragon] [side = mantis] [red starts]")
{
    std::vector<std::string> cards = {"monkey", "tiger", "crab", "dragon", "mantis"};
    std::vector<unsigned long long> perftValues = {11, 143, 1807, 23949, 325011, 4619275, 64873385, 949933361, 13734198945};
    runPerftChecks(cards, 1, perftValues);
}
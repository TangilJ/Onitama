#pragma ide diagnostic ignored "hicpp-signed-bitwise"

#include <map>

#include <bitset>
#include "main.h"
#include "data.h"
#include "perft.h"
#include "utilities.h"
#include "movegen.h"

std::map<std::string, MoveLookup> cardMap = {
    {"ox",       oxMoves},
    {"boar",     boarMoves},
    {"horse",    horseMoves},
    {"elephant", elephantMoves},
    {"crab",     crabMoves}
};


int main()
{
    constexpr Bitboard startingBoardP0 = 0b11111000000000000000000000000011;
    constexpr Bitboard startingBoardP1 = 0b00000000000000000000111110001100;
    constexpr Bitboard startingBoardKings = 0b00100000000000000000001000010000;

    State startState = {{startingBoardP0, startingBoardP1}, startingBoardKings};
    printIncreasingPerftSpeed(startState, 8, 0);

    std::cout << "Finished. Press enter to exit." << std::endl;
    getchar();

    return 0;
}

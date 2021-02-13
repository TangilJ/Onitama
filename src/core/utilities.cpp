#include <cstdlib>
#include <bitset>
#include <intrin.h>
#include <vector>
#include <iterator>
#include <algorithm>
#include "utilities.h"
#include "data.h"

void printBits(Bitboard board)
{
    std::bitset<32> x(board);
    std::cout << x << std::endl;
}

void printBoard(State state)
{
    Bitboard currentSquare = firstSquare;
    for (int i = 0; i < 25; ++i) {
        if (currentSquare & state.allPieces[0])
            if (currentSquare & state.kings)
                std::cout << "B";
            else
                std::cout << "b";
        else if (currentSquare & state.allPieces[1])
            if (currentSquare & state.kings)
                std::cout << "R";
            else
                std::cout << "r";
        else
            std::cout << ".";
        std::cout << " ";

        if ((i + 1) % 5 == 0)
            std::cout << i / 5 + 1 << std::endl;

        currentSquare >>= 1;
    }

    for (int i = 4; i >= 0; --i)
        printf("%c ", 65 + i);

    int blue0 = _bit_scan_forward(state.allPieces[0]);
    int blue1 = _bit_scan_forward(state.allPieces[0] ^ (1 << blue0));
    int red0 = _bit_scan_forward(state.allPieces[1]);
    int red1 = _bit_scan_forward(state.allPieces[1] ^ (1 << red0));
    int side = _bit_scan_forward(state.kings);
    printf("\nblue = {%i, %i}, red = {%i, %i}, side = %i\n", blue0, blue1, red0, red1, side);
}

void printBoard(Bitboard board)
{
    printBoard({board, 0, 0});
}

void getLookupsFromNames(std::vector<std::string> names, MoveLookup *lookups)
{
    for (int i = 0; i < 5; ++i) {
        if (cardMap.find(names[i]) != cardMap.end())
            lookups[i] = cardMap.at(names[i]);
        else
            throw std::invalid_argument("Name not found in cardMap");
    }
}

void getRandomCards(MoveLookup *lookups)
{

    std::vector<int> indices;
    while (indices.size() != 5) {
        int i = rand() % 16;
        if (std::find(indices.begin(), indices.end(), i) == indices.end())
            indices.push_back(i);
    }

    int nameIndex = 0;
    int lookupsIndex = 0;
    for (const auto &name : cardMap) {
        if (std::find(indices.begin(), indices.end(), nameIndex) != indices.end()) {
            lookups[lookupsIndex] = cardMap.at(name.first);
            lookupsIndex++;
        }
        nameIndex++;
    }
}

#include <cstdlib>
#include <bitset>
#include <intrin.h>
#include <vector>
#include <iterator>
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

    for (int i = 0; i < 5; ++i)
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

void getStateFromServerString(const std::string &string, State &state)
{
    // @formatter:off
    state.allPieces[0]  = 0b00011;
    state.allPieces[1]  = 0b01100;
    state.kings         = 0b10000;
    // @formatter:on

    for (int i = 0; i < 25; ++i) {
        if (string[i] == '1' or string[i] == '2')
            state.allPieces[0] |= firstSquare >> i;
        if (string[i] == '3' or string[i] == '4')
            state.allPieces[1] |= firstSquare >> i;
        if (string[i] == '2' or string[i] == '4')
            state.kings |= firstSquare >> i;
    }
}

std::string serverMoveStringFromStates(State first, State second, std::vector<std::string> names)
{
    Bitboard blueMove = ~(first.allPieces[0] & boardMask) & (second.allPieces[0] & boardMask);
    Bitboard redMove = ~(first.allPieces[1] & boardMask) & (second.allPieces[1] & boardMask);
    Bitboard move;
    Bitboard original;

    puts("Blue:");
    printBits(first.allPieces[0] & boardMask);
    printBits(second.allPieces[0] & boardMask);

    puts("Red:");
    printBits(first.allPieces[1] & boardMask);
    printBits(second.allPieces[1] & boardMask);

    puts("Moves:");
    printBits(blueMove);
    printBits(redMove);

    if (blueMove != 0) {
        move = blueMove;
        original = ~(second.allPieces[0] & boardMask) & (first.allPieces[0] & boardMask);
        puts("Blue original:");
        printBits(original);
    }
    else if (redMove != 0) {
        move = redMove;
        original = ~(second.allPieces[1] & boardMask) & (first.allPieces[1] & boardMask);
        puts("Red original:");
        printBits(original);
    }
    else
        throw std::invalid_argument("No move detected between states");

    int moveIndex = 31 - _bit_scan_forward(move);
    printf("moveIndex: %i\n", moveIndex);
    char moveRank = 97 + moveIndex % 5;
    int moveFile = moveIndex / 5 + 1;

    int originalIndex = 31 - _bit_scan_forward(original);
    printf("originalIndex: %i\n", originalIndex);
    char originalRank = 97 + originalIndex % 5;
    int originalFile = originalIndex / 5 + 1;

    std::string cardUsed = names[_bit_scan_forward(second.kings)];

    char buffer[50];
    sprintf(buffer, "%s %c%i%c%i", cardUsed.c_str(), originalRank, originalFile, moveRank, moveFile);
    return std::string(buffer);
}

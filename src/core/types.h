#ifndef ONITAMA_TYPES_H
#define ONITAMA_TYPES_H

#include <iostream>
#include <array>
#include <vector>

using Bitboard = uint32_t;

struct State {
    // Element 0 is blue, element 1 is red
    Bitboard allPieces[2];
    Bitboard kings;
};

using MoveLookup = std::array<std::array<Bitboard, 25>, 2>;
using StateArray = std::array<State, 40>;

#endif //ONITAMA_TYPES_H

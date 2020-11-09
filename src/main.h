#ifndef ONITAMA_MAIN_H
#define ONITAMA_MAIN_H

#include <iostream>
#include <array>

using Bitboard = uint32_t;
constexpr Bitboard firstSquare = 0b10000000000000000000000000000000;
constexpr Bitboard boardMask = 0b11111111111111111111111110000000;
constexpr Bitboard centerSquares = 0b00000011100111001110000000000000;
constexpr Bitboard centralSquare = 0b00000000000010000000000000000000;
constexpr Bitboard blueStartingSquare = 0b00100000000000000000000000000000;
constexpr Bitboard redStartingSquare = 0b00000000000000000000001000000000;

struct State {
    // Element 0 is blue, element 1 is red
    Bitboard allPieces[2];
    Bitboard kings;
};
using MoveLookup = std::array<std::array<Bitboard, 25>, 2>;
using StateArray = std::array<State, 40>;

#endif //ONITAMA_MAIN_H

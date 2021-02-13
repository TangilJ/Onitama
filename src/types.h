#ifndef ONITAMA_TYPES_H
#define ONITAMA_TYPES_H

#include <iostream>
#include <array>
#include <vector>

using Bitboard = uint32_t;

// @formatter:off
constexpr Bitboard firstSquare        = 0b10000'00000'00000'00000'00000'0000000;
constexpr Bitboard boardMask          = 0b11111'11111'11111'11111'11111'0000000;
constexpr Bitboard centerSquares      = 0b00000'01110'01110'01110'00000'0000000;
constexpr Bitboard centralSquare      = 0b00000'00000'00100'00000'00000'0000000;
constexpr Bitboard blueStartingSquare = 0b00100'00000'00000'00000'00000'0000000;
constexpr Bitboard redStartingSquare  = 0b00000'00000'00000'00000'00100'0000000;
constexpr Bitboard blueStartingBoard  = 0b11111'00000'00000'00000'00000'0000011;
constexpr Bitboard redStartingBoard   = 0b00000'00000'00000'00000'11111'0001100;
constexpr Bitboard kingsStartingBoard = 0b00100'00000'00000'00000'00100'0010000;
// @formatter:on

struct State {
    // Element 0 is blue, element 1 is red
    Bitboard allPieces[2];
    Bitboard kings;
};

using MoveLookup = std::array<std::array<Bitboard, 25>, 2>;
using StateArray = std::array<State, 40>;

#endif //ONITAMA_TYPES_H

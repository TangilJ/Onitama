#include "main.h"
#include "utilities.h"

ZobristKey getRandomBits()
{
    return
        (ZobristKey) rand() << 0 & 0xFFFF |
        (ZobristKey) rand() << 16 & 0xFFFF0000;
}

ZobristTable initZobrist()
{
    ZobristTable table;
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 3; ++j)
            table[i][j] = getRandomBits();

    return table;
}

ZobristKey hashZobrist(State state, ZobristTable &table)
{
    ZobristKey hash = 0;
    for (Bitboard i = 0; i < 32; ++i) {
        Bitboard square = 1 << i;
        if (state.allPieces[0] & square)
            hash ^= table[i][0];
        if (state.allPieces[1] & square)
            hash ^= table[i][1];
        if (state.kings & square)
            hash ^= table[i][2];
    }

    return hash;
}

#ifndef ONITAMA_ZOBRIST_H
#define ONITAMA_ZOBRIST_H

#include "main.h"

ZobristTable initZobrist();

ZobristKey hashZobrist(State state, ZobristTable &table);

#endif //ONITAMA_ZOBRIST_H

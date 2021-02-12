#ifndef ONITAMA_PERFTMODE_H
#define ONITAMA_PERFTMODE_H

#include <algorithm>
#include "../main.h"
#include "../utilities.h"
#include "../perft.h"
#include "Mode.h"


struct PerftMode : Mode {
    bool increasingPerft{false};
    bool parallelPerft{false};

    void run() override;
};


#endif //ONITAMA_PERFTMODE_H

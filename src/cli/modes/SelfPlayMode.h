#ifndef ONITAMA_SELFPLAYMODE_H
#define ONITAMA_SELFPLAYMODE_H

#include <algorithm>
#include <cmath>
#include "../../core/types.h"
#include "../../core/utilities.h"
#include "../../core/movegen.h"
#include "../../core/search.h"
#include "../../core/utilities.h"
#include "Mode.h"

struct SelfPlayMode : Mode {
    void run() override;
};


#endif //ONITAMA_SELFPLAYMODE_H

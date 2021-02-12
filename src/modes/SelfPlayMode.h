#ifndef ONITAMA_SELFPLAYMODE_H
#define ONITAMA_SELFPLAYMODE_H

#include <algorithm>
#include <cmath>
#include "../main.h"
#include "../utilities.h"
#include "../movegen.h"
#include "../search.h"
#include "../utilities.h"
#include "Mode.h"

struct SelfPlayMode : Mode {
    void run() override;
};


#endif //ONITAMA_SELFPLAYMODE_H

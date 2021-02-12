#ifndef ONITAMA_SERVERPLAYMODE_H
#define ONITAMA_SERVERPLAYMODE_H

#include <algorithm>
#include <cmath>
#include <easywsclient.hpp>
#include <nlohmann/json.hpp>
#include "../main.h"
#include "../utilities.h"
#include "../movegen.h"
#include "../search.h"
#include "../utilities.h"
#include "Mode.h"

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )

#include <WinSock2.h>
#include <iomanip>

#endif

using json = nlohmann::json;

struct ServerPlayMode : Mode {
    bool printBoardOption{false};
    bool printReceivedPackets{false};
    bool serverCreateMatch{false};
    bool localServerUrl{false};
    std::string serverMatchId;
    std::string serverUrl = "ws://litama.herokuapp.com";

    void processJsonState(json j, MoveLookup *lookups, State &state, int &turn, int &color, int &index, bool &firstPacket);

    void run() override;
};


#endif //ONITAMA_SERVERPLAYMODE_H

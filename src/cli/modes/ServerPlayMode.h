#ifndef ONITAMA_SERVERPLAYMODE_H
#define ONITAMA_SERVERPLAYMODE_H

#include <nlohmann/json.hpp>
#include <core/types.h>

#include "Mode.h"


using json = nlohmann::json;

struct ServerPlayMode : Mode {
    bool printBoardOption{false};
    bool printReceivedPackets{false};
    bool serverCreateMatch{false};
    bool localServerUrl{false};
    std::string serverMatchId;
    std::string serverUrl = "ws://litama.herokuapp.com";

    void run() override;

private:
    void processJsonState(json j, MoveLookup *lookups, State &state, int &turn, int &color, int &index, bool &firstPacket);

    void getStateFromServerString(const std::string &string, State &state);

    std::string serverMoveStringFromStates(State first, State second, std::vector<std::string> names);
};


#endif //ONITAMA_SERVERPLAYMODE_H

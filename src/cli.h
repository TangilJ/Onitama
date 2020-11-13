#ifndef ONITAMA_CLI_H
#define ONITAMA_CLI_H

#include <nlohmann/json.hpp>
#include "main.h"

using json = nlohmann::json;

void perftCommand(CliOptions &options);

void selfPlayCommand(CliOptions &options);

void serverCommand(CliOptions &options);

void humanCommand(CliOptions &options);

void processJsonState(CliOptions &options, json j, MoveLookup *lookups, State &state, int &turn);

#endif //ONITAMA_CLI_H

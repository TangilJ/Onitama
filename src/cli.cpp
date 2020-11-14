#include <nlohmann/json.hpp>
#include <easywsclient.hpp>
#include "main.h"
#include "cli.h"
#include "perft.h"
#include "utilities.h"
#include "movegen.h"
#include "search.h"

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )

#include <WinSock2.h>
#include <iomanip>

#endif

using json = nlohmann::json;
using easywsclient::WebSocket;

void perftCommand(CliOptions &options)
{
    printf("Running perft with depth %i\n\n", options.depth);

    if (std::find(options.cards.begin(), options.cards.end(), "") != options.cards.end())
        options.cards = {"ox", "boar", "horse", "elephant", "crab"};
    MoveLookup lookupsArray[5];
    getLookupsFromNames(options.cards, lookupsArray);

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    if (options.increasingPerft)
        printIncreasingPerftSpeed(state, options.depth, 0, lookupsArray, options.parallelPerft);
    else
        printPerftSpeed(state, options.depth, 0, lookupsArray, options.parallelPerft);
}

void selfPlayCommand(CliOptions &options)
{
    MoveLookup lookups[5];
    if (std::find(options.cards.begin(), options.cards.end(), "") != options.cards.end())
        getRandomCards(lookups);
    else
        getLookupsFromNames(options.cards, lookups);

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    printf("Running self-play with negamax with alpha-beta pruning on depth %i\n\n", options.depth);

    puts("Ply 0");
    printBoard(state);
    puts("");

    int color = 0;
    int ply = 0;
    while (checkWinCondition(state) == -1) {
        const SearchValue &negamax = negamaxWithAbPruning(state, lookups, -INFINITY, INFINITY, options.depth, color, true);
        state = negamax.state;
        color = 1 - color;
        ply++;

        printf("Ply %i\n", ply);
        printBoard(state);
        printf("Evaluation: %.2f\n\n", negamax.value);
    }
}

void serverCommand(CliOptions &options)
{
#ifdef _WIN32
    INT rc;
    WSADATA wsaData;

    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc) {
        printf("WSAStartup Failed.\n");
        return;
    }
#endif

    if (options.localServerUrl)
        options.serverUrl = "ws://127.0.0.1:5000";

    std::unique_ptr<WebSocket> ws(WebSocket::from_url(options.serverUrl));

    std::string token;
    int color;
    int turn = -1;
    State state{};
    MoveLookup lookups[5];
    bool firstPacket = true;

    if (options.serverCreateMatch) {
        ws->send("create");
        puts("Sent: create");
    }
    else if (!options.serverMatchId.empty()) {
        ws->send("join " + options.serverMatchId);
        printf("Sent: join %s", options.serverMatchId.c_str());
    }
    else {
        puts("Did not enter -c or -i for server subcommand. Type 'Onitama.exe server --help' to see options.");
        return;
    }

    while (ws->getReadyState() != WebSocket::CLOSED) {
        ws->poll();
        json data;
        ws->dispatch([&](const std::string &message) {
            data = json::parse(message);
        });

        if (data.find("token") == data.end())
            continue;

        if (options.printReceivedPackets)
            std::cout << "Received: " << std::setw(4) << data << std::endl << std::endl;

        options.serverMatchId = data.at("matchId");
        token = data.at("token");
        color = data.at("color") == "blue" ? 0 : 1;
        printf("matchId: %s\ntoken: %s\ncolor: %i\n", options.serverMatchId.c_str(), token.c_str(), color);
        break;
    }

    ws->send("spectate " + options.serverMatchId);
    while (ws->getReadyState() != WebSocket::CLOSED) {
        if (color == turn) {
            std::clock_t start = std::clock();
            const SearchValue &search = negamaxWithAbPruning(state, lookups, -INFINITY, INFINITY, options.depth, color, true);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

            puts("Sending board:");
            printBoard(search.state);
            printf("Evaluation: %f, took %fs\n", search.value, duration);

            std::string serverMove = serverMoveStringFromStates(state, search.state, options.cards);
            std::string sentCommand = "move " + options.serverMatchId + " " + token + " " + serverMove;
            printf("Sent: %s\n\n", sentCommand.c_str());

            ws->send(sentCommand);
            turn = -1;
        }

        ws->poll();
        json data;
        ws->dispatch([&](const std::string &message) {
            data = json::parse(message);
        });

        if (data.find("messageType") == data.end())
            continue;

        if (options.printReceivedPackets)
            std::cout << "Received: " << std::setw(4) << data << std::endl << std::endl;

        if (data.at("messageType") == "state") {
            if (data.at("gameState") == "in progress") {
                processJsonState(options, data, lookups, state, turn, firstPacket);
                puts("Current board:");
                printBoard(state);
                std::cout << std::endl;
            }
            else if (data.at("gameState") == "ended") {
                processJsonState(options, data, lookups, state, turn, firstPacket);
                puts("Final board:");
                printBoard(state);
                std::cout << "Game ended" << std::endl;
                break;
            }
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif
}

void humanCommand(CliOptions &options)
{
    puts("Playing against the AI is currently not supported.");
}

void processJsonState(CliOptions &options, json j, MoveLookup *lookups, State &state, int &turn, bool &firstPacket)
{
    turn = j.at("currentTurn") == "blue" ? 0 : 1;
    getStateFromServerString(j.at("board"), state);
    if (firstPacket) {
        // Add the new cards to the bitboards and get the lookup tables
        options.cards = {
            j.at("cards").at("blue")[0],
            j.at("cards").at("blue")[1],
            j.at("cards").at("red")[0],
            j.at("cards").at("red")[1],
            j.at("cards").at("side")
        };
        getLookupsFromNames(options.cards, lookups);
        // @formatter:off
        state.allPieces[0]  = 0b00011;
        state.allPieces[1]  = 0b01100;
        state.kings         = 0b10000;
        // @formatter:on
        firstPacket = false;
    }
    else
    {
        // Update the cards in the bitboards so that they match the lookup tables we already made on the first packet
        for (int i = 0; i < 2; ++i) {
            std::string name = j.at("cards").at("blue")[i];
            int index = std::find(options.cards.begin(), options.cards.end(), name) - options.cards.begin();
            state.allPieces[0] |= 1 << index;
        }
        for (int i = 0; i < 2; ++i) {
            std::string name = j.at("cards").at("red")[i];
            int index = std::find(options.cards.begin(), options.cards.end(), name) - options.cards.begin();
            state.allPieces[1] |= 1 << index;
        }
        std::string name = j.at("cards").at("side");
        int index = std::find(options.cards.begin(), options.cards.end(), name) - options.cards.begin();
        state.kings |= 1 << index;
    }
}

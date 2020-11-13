#ifdef _WIN32
#pragma comment( lib, "ws2_32" )

#include <WinSock2.h>

#endif

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>
#include <easywsclient.hpp>
#include "main.h"
#include "data.h"
#include "perft.h"
#include "utilities.h"
#include "movegen.h"
#include "search.h"

using easywsclient::WebSocket;
using json = nlohmann::json;

struct CardNameValidator : public CLI::Validator {
    CardNameValidator()
    {
        func_ = [](const std::string &str) {
            if (cardMap.find(str) == cardMap.end())
                return std::string("Card '") + str + std::string("' does not exist.");
            else
                return std::string();
        };
    }
};

const static CardNameValidator cardNameValidator;

std::vector<std::string> cards = {"", "", "", "", ""};
int depth = 1;
bool increasingPerft{false};
bool serverCreateMatch{false};
bool localServerUrl{false};
bool printReceivedPackets{false};
std::string serverMatchId;
std::string serverUrl = "ws://litama.herokuapp.com";


void perftCommand()
{
    printf("Running perft with depth %i\n\n", depth);

    if (std::find(cards.begin(), cards.end(), "") != cards.end())
        cards = {"ox", "boar", "horse", "elephant", "crab"};
    MoveLookup lookupsArray[5];
    getLookupsFromNames(cards, lookupsArray);

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    if (increasingPerft)
        printIncreasingPerftSpeed(state, depth, 0, lookupsArray);
    else
        printPerftSpeed(state, depth, 0, lookupsArray);
}

void selfPlayCommand()
{
    MoveLookup lookups[5];
    if (std::find(cards.begin(), cards.end(), "") != cards.end())
        getRandomCards(lookups);
    else
        getLookupsFromNames(cards, lookups);

    State state = {
        {blueStartingBoard, redStartingBoard},
        kingsStartingBoard
    };

    printf("Running self-play with negamax with alpha-beta pruning on depth %i\n\n", depth);

    puts("Ply 0");
    printBoard(state);
    puts("");

    int color = 0;
    int ply = 0;
    while (checkWinCondition(state) == -1) {
        const SearchValue &negamax = negamaxWithAbPruning(state, lookups, -INFINITY, INFINITY, depth, color, true);
        state = negamax.state;
        color = 1 - color;
        ply++;

        printf("Ply %i\n", ply);
        printBoard(state);
        printf("Evaluation: %.2f\n\n", negamax.value);
    }
}

void processJsonState(json j, MoveLookup *lookups, State &state, int &turn)
{
    turn = j.at("currentTurn") == "blue" ? 0 : 1;
    getStateFromServerString(j.at("board"), state);
    cards = {
        j.at("cards").at("blue")[0],
        j.at("cards").at("blue")[1],
        j.at("cards").at("red")[0],
        j.at("cards").at("red")[1],
        j.at("cards").at("side")
    };
    getLookupsFromNames(cards, lookups);
}

void serverCommand()
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

    if (localServerUrl)
        serverUrl = "ws://127.0.0.1:5000";

    std::unique_ptr<WebSocket> ws(WebSocket::from_url(serverUrl));

    std::string token;
    int color;
    int turn = -1;
    State state{};
    MoveLookup lookups[5];

    if (serverCreateMatch) {
        ws->send("create");
        puts("Sent: create");
    }
    else if (!serverMatchId.empty()) {
        ws->send("join " + serverMatchId);
        printf("Sent: join %s", serverMatchId.c_str());
    }
    else {
        puts("Did not enter any options for server subcommand. Type 'Onitama.exe server --help' to see options.");
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

        if (printReceivedPackets)
            std::cout << "Received: " << std::setw(4) << data << std::endl << std::endl;

        serverMatchId = data.at("matchId");
        token = data.at("token");
        color = data.at("color") == "blue" ? 0 : 1;
        printf("matchId: %s\ntoken: %s\ncolor: %i\n", serverMatchId.c_str(), token.c_str(), color);
        break;
    }

    ws->send("spectate " + serverMatchId);
    while (ws->getReadyState() != WebSocket::CLOSED) {
        if (color == turn) {
            std::clock_t start = std::clock();
            const SearchValue &search = negamaxWithAbPruning(state, lookups, -INFINITY, INFINITY, depth, color, true);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
            puts("Sending board:");
            printBoard(search.state);
            printf("Evaluation: %f, took %fs\n", search.value, duration);
            std::string serverMove = serverMoveStringFromStates(state, search.state, cards);
            std::string sentCommand = "move " + serverMatchId + " " + token + " " + serverMove;
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

        if (printReceivedPackets)
            std::cout << "Received: " << std::setw(4) << data << std::endl << std::endl;

        if (data.at("messageType") == "state") {
            if (data.at("gameState") == "in progress") {
                processJsonState(data, lookups, state, turn);
                puts("Current board:");
                printBoard(state);
                std::cout << std::endl;
            }
            else if (data.at("gameState") == "ended") {
                processJsonState(data, lookups, state, turn);
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

int main(int argc, char **argv)
{
    CLI::App app;
    app.require_subcommand(0, 1);


    CLI::App *perftSubcommand = app.add_subcommand(
            "perft",
            "Run a parallelised perft (performance test).")
        ->ignore_case();
    perftSubcommand->add_option("-d,--depth", depth, "Perft depth.", true);
    perftSubcommand->add_flag("-i,--increasing", increasingPerft, "Make the perft increase depth-by-depth to the given depth.");
    perftSubcommand->add_option(
        "-c,--cards", cards,
        "The 5 cards to be used in perft in order of: blue1, blue2, red1, red2, side. "
        "Defaults to blue = ox, boar, red = horse, elephant, side = crab."
    )->ignore_case()->expected(5)->check(cardNameValidator);
    perftSubcommand->callback(perftCommand);


    CLI::App *selfPlay = app.add_subcommand("self", "Make the AI play against itself.")
        ->ignore_case();
    selfPlay->add_option(
        "-c,--cards", cards,
        "The 5 cards to be used in a self-play game in order of: blue1, blue2, red1, red2, side "
        "(e.g. ox, boar, elephant, horse, crab). "
        "Leave this option out for random cards."
    )->ignore_case()->expected(5)->check(cardNameValidator);
    selfPlay->add_option("-d,--depth", depth, "The number of plies that the negamax algorithm will look ahead.", true);
    selfPlay->callback(selfPlayCommand);


    CLI::App *againstHuman = app.add_subcommand("human", "Make the AI play against a human.")
        ->ignore_case();
    againstHuman->add_option(
        "-c,--cards", cards,
        "The 5 cards to be used in a game against humans in order of: blue1, blue2, red1, red2, side "
        "(e.g. ox, boar, elephant, horse, crab). "
        "Leave this option out for random cards."
    )->ignore_case()->expected(5)->check(cardNameValidator);


    CLI::App *litamaServer = app.add_subcommand("server", "Make the AI play on the Litama server.")
        ->ignore_case();
    litamaServer->require_option(1, 3);
    litamaServer->add_option("-d,--depth", depth, "The number of plies to look ahead when playing on the server.", true);
    litamaServer->add_option(
        "-i,--matchId", serverMatchId,
        "The match ID to connect to if using the Litama server."
    )->ignore_case();
    litamaServer->add_option(
        "-u,--url", serverUrl,
        "The URL to connect to for the Litama server.",
        true
    )->ignore_case();
    litamaServer->add_flag(
        "-l,--local", localServerUrl,
        "Connects to ws://127.0.0.1 instead of the default URL."
    )->ignore_case();
    litamaServer->add_flag(
        "-c,--create", serverCreateMatch,
        "Create a new match on the Litama server."
    )->ignore_case();
    litamaServer->add_flag(
        "-p,--packets", printReceivedPackets,
        "Print the JSON packets received from the Litama server."
    )->ignore_case();
    litamaServer->callback(serverCommand);


    app.footer("Run --help on a subcommand to see its options.\n"
               "Onitama engine written in C++ by Tangil Jahangir.");


    CLI11_PARSE(app, argc, argv);

    return 0;
}

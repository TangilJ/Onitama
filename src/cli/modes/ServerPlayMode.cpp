#include "ServerPlayMode.h"

#include <algorithm>
#include <cmath>
#include <easywsclient.hpp>

#ifdef _WIN32
#pragma comment( lib, "ws2_32" )

#include <winsock2.h>
#include <iomanip>

#endif

#include <core/data.h>
#include <core/utilities.h>
#include <core/search.h>


using easywsclient::WebSocket;


void ServerPlayMode::run()
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

    if (this->localServerUrl)
        this->serverUrl = "ws://127.0.0.1:5000";

    std::unique_ptr<WebSocket> ws(WebSocket::from_url(this->serverUrl));

    std::string token;
    int index;
    int turn = -1;
    int color = -2;
    State state{};
    MoveLookup lookups[5];
    bool firstPacket = true;

    if (this->serverCreateMatch) {
        ws->send("create Bot");
        puts("Sent: create Bot");
    }
    else if (!this->serverMatchId.empty()) {
        const std::basic_string<char> &message = "join " + this->serverMatchId + " Bot";
        ws->send(message);
        printf("Sent: %s\n", message.c_str());
    }
    else {
        puts("Did not enter -c or -i for server subcommand. Type 'Onitama.exe server --help' to see this->");
        return;
    }

    while (ws->getReadyState() != WebSocket::CLOSED) {
        ws->poll();
        json data;
        ws->dispatch([&](const std::string &message) {
            data = json::parse(message);
        });

        if (data.find("messageType") == data.end())
            continue;

        if (data.at("messageType") == "error") {
            puts("Received error:");
            std::cout << std::setw(4) << data << std::endl << std::endl;
            puts("Exiting.");
            return;
        }

        if (this->printReceivedPackets)
            std::cout << "Received: " << std::setw(4) << data << std::endl << std::endl;

        this->serverMatchId = data.at("matchId");
        token = data.at("token");
        index = data.at("index");
        printf("matchId: %s\ntoken: %s\nindex: %i\n", this->serverMatchId.c_str(), token.c_str(), index);
        break;
    }

    ws->send("spectate " + this->serverMatchId);
    while (ws->getReadyState() != WebSocket::CLOSED) {
        if (color == turn) {
            std::clock_t start = std::clock();
            const SearchValue &search = negamaxWithAbPruning(state, lookups, -INFINITY, INFINITY, this->depth, color, true);
            double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

            if (this->printBoardOption) {
                puts("Sending board:");
                printBoard(search.state);
            }
            printf("Evaluation: %f, took %fs\n", search.value, duration);

            std::string serverMove = serverMoveStringFromStates(state, search.state, this->cards);
            std::string sentCommand = "move " + this->serverMatchId + " " + token + " " + serverMove;
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

        if (this->printReceivedPackets)
            // No need to print the error twice since errors will always be printed,
            // regardless of this->printReceivedPackets
            if (data.at("messageType") != "error")
                std::cout << "Received: " << std::setw(4) << data << std::endl << std::endl;

        if (data.at("messageType") == "state") {
            if (data.at("gameState") == "in progress") {
                processJsonState(data, lookups, state, turn, color, index, firstPacket);
                if (this->printBoardOption) {
                    puts("Current board:");
                    printBoard(state);
                    std::cout << std::endl;
                }
            }
            else if (data.at("gameState") == "ended") {
                processJsonState(data, lookups, state, turn, color, index, firstPacket);
                if (this->printBoardOption) {
                    puts("Final board:");
                    printBoard(state);
                }
                std::cout << "Game ended. ";
                if (data.at("winner") == "blue" && color == 0 || data.at("winner") == "red" && color == 1)
                    std::cout << "Won game." << std::endl;
                else
                    std::cout << "Lost game." << std::endl;
                    
                break;
            }
        }
        else if (data.at("messageType") == "error") {
            // Print the packet even if this->printReceivedPackets is false since an error is a big issue.
            puts("Received error:");
            std::cout << std::setw(4) << data << std::endl << std::endl;
        }
    }

#ifdef _WIN32
    WSACleanup();
#endif
}


void ServerPlayMode::processJsonState(
    json j, MoveLookup *lookups, State &state, int &turn, int &color, int &index, bool &firstPacket)
{
    turn = j.at("currentTurn") == "blue" ? 0 : 1;
    color = j.at("indices").at("blue") == index ? 0 : 1;
    getStateFromServerString(j.at("board"), state);
    if (firstPacket) {
        // Add the new cards to the bitboards and get the lookup tables
        this->cards = {
            j.at("cards").at("blue")[0],
            j.at("cards").at("blue")[1],
            j.at("cards").at("red")[0],
            j.at("cards").at("red")[1],
            j.at("cards").at("side")
        };
        getLookupsFromNames(this->cards, lookups);
        // @formatter:off
        state.allPieces[0]  |= 0b00011;
        state.allPieces[1]  |= 0b01100;
        state.kings         |= 0b10000;
        // @formatter:on
        firstPacket = false;
    }
    else {
        // Update the cards in the bitboards so that they match the lookup tables we already made on the first packet
        for (int i = 0; i < 2; ++i) {
            std::string name = j.at("cards").at("blue")[i];
            int index = std::find(this->cards.begin(), this->cards.end(), name) - this->cards.begin();
            state.allPieces[0] |= 1 << index;
        }
        for (int i = 0; i < 2; ++i) {
            std::string name = j.at("cards").at("red")[i];
            int index = std::find(this->cards.begin(), this->cards.end(), name) - this->cards.begin();
            state.allPieces[1] |= 1 << index;
        }
        std::string name = j.at("cards").at("side");
        int index = std::find(this->cards.begin(), this->cards.end(), name) - this->cards.begin();
        state.kings |= 1 << index;
    }
}


void ServerPlayMode::getStateFromServerString(const std::string &string, State &state)
{
    state.allPieces[0] = 0;
    state.allPieces[1] = 0;
    state.kings = 0;

    int count = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j <= 4; ++j) {
            int index = i * 5 + (4 - j);
            if (string[index] == '1' || string[index] == '2')
                state.allPieces[0] |= firstSquare >> count;
            if (string[index] == '3' || string[index] == '4')
                state.allPieces[1] |= firstSquare >> count;
            if (string[index] == '2' || string[index] == '4')
                state.kings |= firstSquare >> count;
            count++;
        }
    }
}

std::string ServerPlayMode::serverMoveStringFromStates(State first, State second, std::vector<std::string> names)
{
    Bitboard blueMove = ~(first.allPieces[0] & boardMask) & (second.allPieces[0] & boardMask);
    Bitboard redMove = ~(first.allPieces[1] & boardMask) & (second.allPieces[1] & boardMask);
    Bitboard move;
    Bitboard original;

    if (blueMove != 0) {
        move = blueMove;
        original = ~(second.allPieces[0] & boardMask) & (first.allPieces[0] & boardMask);
    }
    else if (redMove != 0) {
        move = redMove;
        original = ~(second.allPieces[1] & boardMask) & (first.allPieces[1] & boardMask);
    }
    else
        throw std::invalid_argument("No move detected between states");

    int moveIndex = 31 - _bit_scan_forward(move);
    char moveRank = 97 + (4 - moveIndex % 5);
    int moveFile = moveIndex / 5 + 1;

    int originalIndex = 31 - _bit_scan_forward(original);
    char originalRank = 97 + (4 - originalIndex % 5);
    int originalFile = originalIndex / 5 + 1;

    std::string cardUsed = names[_bit_scan_forward(second.kings)];

    char buffer[50];
    sprintf(buffer, "%s %c%i%c%i", cardUsed.c_str(), originalRank, originalFile, moveRank, moveFile);
    return std::string(buffer);
}
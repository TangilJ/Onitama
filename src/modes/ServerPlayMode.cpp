#include "ServerPlayMode.h"


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
        printf("Sent: %s", message.c_str());
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
                std::cout << "Game ended" << std::endl;
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

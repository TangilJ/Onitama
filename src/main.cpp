#include <CLI/CLI.hpp>
#include "main.h"
#include "data.h"

#include "modes/Mode.h"
#include "modes/PerftMode.h"
#include "modes/SelfPlayMode.h"
#include "modes/ServerPlayMode.h"
#include "modes/HumanPlayMode.h"


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

int main(int argc, char **argv)
{
    CLI::App app;
    app.require_subcommand(0, 1);

    PerftMode perftMode;
    CLI::App *perftSubcommand = app.add_subcommand(
        "perft",
        "Run perft (performance test)."
    )->ignore_case();
    perftSubcommand->add_option(
        "-d,--depth", perftMode.depth,
        "Perft depth.",
        true
    )->ignore_case();
    perftSubcommand->add_flag(
        "-p,--parallel", perftMode.parallelPerftOption,
        "Parallelise perft."
    )->ignore_case();
    perftSubcommand->add_flag(
        "-i,--increasing", perftMode.increasingPerft,
        "Make the perft increase depth-by-depth to the given depth."
    )->ignore_case();
    perftSubcommand->add_option(
        "-c,--cards", perftMode.cards,
        "The 5 cards to be used in perft in order of: blue1, blue2, red1, red2, side. "
        "Defaults to blue = ox, boar, red = horse, elephant, side = crab."
    )->ignore_case()->expected(5)->check(cardNameValidator);
    perftSubcommand->add_option(
      "-s,--start", perftMode.startingPlayer,
      "The index of the player that will start (0 = blue, 1 = red)."
    )->ignore_case()->check(CLI::Range(0, 1));
    perftSubcommand->callback([&]() {
        perftMode.run();
    });


    SelfPlayMode selfPlayMode;
    CLI::App *selfPlay = app.add_subcommand(
        "self",
        "Make the AI play against itself."
    )->ignore_case();
    selfPlay->add_option(
        "-c,--cards", selfPlayMode.cards,
        "The 5 cards to be used in a self-play game in order of: blue1, blue2, red1, red2, side "
        "(e.g. ox, boar, elephant, horse, crab). "
        "Leave this option out for random cards."
    )->ignore_case()->expected(5)->check(cardNameValidator);
    selfPlay->add_option(
        "-d,--depth", selfPlayMode.depth,
        "The number of plies that the negamax algorithm will look ahead.",
        true
    )->ignore_case();
    selfPlay->callback([&]() {
        selfPlayMode.run();
    });


    HumanPlayMode humanPlayMode;
    CLI::App *againstHuman = app.add_subcommand(
        "human",
        "Make the AI play against a human."
    )->ignore_case();
    againstHuman->add_option(
        "-c,--cards", humanPlayMode.cards,
        "The 5 cards to be used in a game against humans in order of: blue1, blue2, red1, red2, side "
        "(e.g. ox, boar, elephant, horse, crab). "
        "Leave this option out for random cards."
    )->ignore_case()->expected(5)->check(cardNameValidator);
    againstHuman->add_option(
        "-d,--depth", humanPlayMode.depth,
        "The number of plies that the negamax algorithm will look ahead.",
        true
    )->ignore_case();
    againstHuman->callback([&]() {
        humanPlayMode.run();
    });


    ServerPlayMode serverPlayMode;
    CLI::App *litamaServer = app.add_subcommand(
        "server",
        "Make the AI play on the Litama server."
    )->ignore_case();
    litamaServer->require_option(1, 3);
    litamaServer->add_option(
        "-d,--depth", serverPlayMode.depth,
        "The number of plies that the negamax algorithm will look ahead.",
        true
    )->ignore_case();
    litamaServer->add_option(
        "-i,--matchId", serverPlayMode.serverMatchId,
        "The match ID to connect to if using the Litama server."
    )->ignore_case();
    litamaServer->add_option(
        "-u,--url", serverPlayMode.serverUrl,
        "The URL to connect to for the Litama server.",
        true
    )->ignore_case();
    litamaServer->add_flag(
        "-l,--local", serverPlayMode.localServerUrl,
        "Connects to ws://127.0.0.1 instead of the default URL."
    )->ignore_case();
    litamaServer->add_flag(
        "-c,--create", serverPlayMode.serverCreateMatch,
        "Create a new match on the Litama server."
    )->ignore_case();
    litamaServer->add_flag(
        "-p,--packets", serverPlayMode.printReceivedPackets,
        "Print the JSON packets received from the Litama server."
    )->ignore_case();
    litamaServer->add_flag(
        "-b,--board", serverPlayMode.printBoardOption,
        "Print the current board, the board that will be sent to the server, and the final board."
    )->ignore_case();
    litamaServer->callback([&]() {
        serverPlayMode.run();
    });


    app.footer("Run --help on a subcommand to see its options.\n"
               "Onitama engine written in C++ by Tangil Jahangir.");


    CLI11_PARSE(app, argc, argv);

    return 0;
}

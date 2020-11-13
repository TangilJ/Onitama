#include <CLI/CLI.hpp>
#include "main.h"
#include "cli.h"
#include "data.h"


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
    CliOptions options{};

    CLI::App app;
    app.require_subcommand(0, 1);


    CLI::App *perftSubcommand = app.add_subcommand(
        "perft",
        "Run a parallelised perft (performance test)."
    )->ignore_case();
    perftSubcommand->add_option("-d,--depth", options.depth, "Perft depth.", true);
    perftSubcommand->add_flag(
        "-i,--increasing", options.increasingPerft,
        "Make the perft increase depth-by-depth to the given depth."
    );
    perftSubcommand->add_option(
        "-c,--cards", options.cards,
        "The 5 cards to be used in perft in order of: blue1, blue2, red1, red2, side. "
        "Defaults to blue = ox, boar, red = horse, elephant, side = crab."
    )->ignore_case()->expected(5)->check(cardNameValidator);
    perftSubcommand->callback([&]() {
        perftCommand(options);
    });


    CLI::App *selfPlay = app.add_subcommand("self", "Make the AI play against itself.")
        ->ignore_case();
    selfPlay->add_option(
        "-c,--cards", options.cards,
        "The 5 cards to be used in a self-play game in order of: blue1, blue2, red1, red2, side "
        "(e.g. ox, boar, elephant, horse, crab). "
        "Leave this option out for random cards."
    )->ignore_case()->expected(5)->check(cardNameValidator);
    selfPlay->add_option("-d,--depth", options.depth, "The number of plies that the negamax algorithm will look ahead.", true);
    selfPlay->callback([&]() {
        selfPlayCommand(options);
    });


    CLI::App *againstHuman = app.add_subcommand("human", "Make the AI play against a human.")
        ->ignore_case();
    againstHuman->add_option(
        "-c,--cards", options.cards,
        "The 5 cards to be used in a game against humans in order of: blue1, blue2, red1, red2, side "
        "(e.g. ox, boar, elephant, horse, crab). "
        "Leave this option out for random cards."
    )->ignore_case()->expected(5)->check(cardNameValidator);


    CLI::App *litamaServer = app.add_subcommand("server", "Make the AI play on the Litama server.")
        ->ignore_case();
    litamaServer->require_option(1, 3);
    litamaServer->add_option("-d,--depth", options.depth, "The number of plies to look ahead when playing on the server.", true);
    litamaServer->add_option(
        "-i,--matchId", options.serverMatchId,
        "The match ID to connect to if using the Litama server."
    )->ignore_case();
    litamaServer->add_option(
        "-u,--url", options.serverUrl,
        "The URL to connect to for the Litama server.",
        true
    )->ignore_case();
    litamaServer->add_flag(
        "-l,--local", options.localServerUrl,
        "Connects to ws://127.0.0.1 instead of the default URL."
    )->ignore_case();
    litamaServer->add_flag(
        "-c,--create", options.serverCreateMatch,
        "Create a new match on the Litama server."
    )->ignore_case();
    litamaServer->add_flag(
        "-p,--packets", options.printReceivedPackets,
        "Print the JSON packets received from the Litama server."
    )->ignore_case();
    litamaServer->callback([&]() {
        serverCommand(options);
    });


    app.footer("Run --help on a subcommand to see its options.\n"
               "Onitama engine written in C++ by Tangil Jahangir.");


    CLI11_PARSE(app, argc, argv);

    return 0;
}

#include <CLI/CLI.hpp>
#include "main.h"
#include "data.h"
#include "perft.h"

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

std::vector<std::string> cards;
int perftDepth = 1;
bool increasingPerft {false};
std::string serverMatchId;

int main(int argc, char **argv)
{
    CLI::App app;

    app.require_subcommand(0, 1);

    CLI::App *perftSubcommand = app.add_subcommand(
            "perft",
            "Run a parallelised perft (performance test).")
        ->ignore_case();
    perftSubcommand->add_option("-d,--depth", perftDepth, "Perft depth.", true);
    perftSubcommand->add_flag("-i,--increasing", increasingPerft, "Make the perft increase depth-by-depth to the given depth.");
    perftSubcommand->add_option(
        "-c,--cards", cards,
        "The 5 cards to be used in a selfplay game in order of: blue1, blue2, red1, red2, side. "
        "Defaults to blue = ox, boar, red = horse, elephant, side = crab."
    )->ignore_case()->expected(5)->check(cardNameValidator);

    CLI::App *selfPlay = app.add_subcommand("self", "Make the AI play against itself.")
        ->ignore_case();
    selfPlay->add_option(
        "-c,--cards", cards,
        "The 5 cards to be used in a selfplay game in order of: blue1, blue2, red1, red2, side "
        "(e.g. ox, boar, elephant, horse, crab). "
        "Leave this option out for random cards."
    )->ignore_case()->expected(5)->check(cardNameValidator);

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
    litamaServer->add_option(
        "-i,--matchId", serverMatchId,
        "The match ID to connect to if using the Litama server."
    )->ignore_case()->required();


    app.footer("Run --help on a subcommand to see its options.\n"
               "Onitama engine written in C++ by Redox.");

    CLI11_PARSE(app, argc, argv);

    return 0;
}

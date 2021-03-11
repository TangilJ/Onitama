# Onitama

This is an AI for the board game called [Onitama](https://www.arcanewonders.com/game/onitama/).

## Features

* Bitboards
* Negamax tree search
* Alpha-beta pruning
* Can play against itself
* Can play against other bots/players on the Litama server
* Perft (performance test)

## Getting the AI

### From Releases

This is the easiest way to get the AI. Simply download the latest version of the AI from the
[Releases](https://github.com/TheBlocks/Onitama/releases) section on the GitHub repository.

### From source

The GitHub repository comes with a `CMakeLists.txt` file so that you can easily compile the project yourself. Make sure
to clone the repository with `--recurse-submodules`
(i.e. `git clone --recurse-submodules https://github.com/TheBlocks/Onitama`) so that you also get all the necessary
dependencies in the `extern/` folder.

The code compiles on both GCC and Clang. Other compilers have not been tested. Note that GCC builds do not support
parallel perft.

## Running

The executable comes with a command line interface with several different options for the AI. Run the executable with
the `-h` flag to see all options.

```
> Onitama.exe -h

Usage: Onitama.exe [OPTIONS] [SUBCOMMAND]

Options:
  -h,--help                   Print this help message and exit
  -v,--version                Print engine version.

Subcommands:
  perft                       Run perft (performance test).
  self                        Make the AI play against itself.
  human                       Make the AI play against a human.
  server                      Make the AI play on the Litama server.
  tests                       Run tests to verify the engine is working correctly.
                              Write extra command line options after 'tests' to pass them to doctest (e.g. Onitama.exe tests -ltc).

Run --help on a subcommand to see its options.
Onitama engine written in C++ by Tangil Jahangir.
```

## Built with

* [CLIUtils/CLI11](https://github.com/CLIUtils/CLI11) - For the command line interface
* [onqtam/doctest](https://github.com/onqtam/doctest) - For running unit tests
* [dhbaird/easywsclient](https://github.com/dhbaird/easywsclient) - For connecting to the Litama server with WebSocket
* [nlohmann/json](https://github.com/nlohmann/json) - For parsing JSON from the Litama server
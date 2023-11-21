#include "game.hpp"
#include "registry.hpp"

#include <cxxopts.hpp>

#include <fstream>

int seed;

void version() {
    std::cout << "Apocalypse Now v2." << std::endl;
    std::cout << "compiled " << __TIME__ << " " << __DATE__ << std::endl;
}

cxxopts::Options getOptions() {
    cxxopts::Options options("Apocalypse Now", "A 2D AI game for Apocalypse Now");

    // clang-format off
    options.add_options()
        ("player", "Players to play", cxxopts::value<std::vector<std::string>>())
        ("s,seed", "set random seed (default: time)", cxxopts::value<int>()->default_value("-1"))
        ("i,input", "set input file  (default: stdin)", cxxopts::value<std::string>()->default_value(""))
        ("o,output", "set output file (default: stdout)", cxxopts::value<std::string>()->default_value(""))
        ("l,list", "list registered players")
        ("v,version", "print version")
        ("h,help", "print help");
    // clang-format on

    options.parse_positional({"player"});
    return options;
}

int main(int argc, char **argv) {

    auto options = getOptions();
    const auto result = options.parse(argc, argv);

    seed = result["seed"].as<int>();
    if (seed < 0) {
        seed = time(0);
        srand(seed);
    }

    std::vector<std::string> names;
    if (result.count("player")) {
        names = result["player"].as<std::vector<std::string>>();
    }

    std::string pathIn = result["input"].as<std::string>();
    std::unique_ptr<std::ifstream> fileIn;
    std::istream *streamIn = &std::cin;
    if (!pathIn.empty()) {
        fileIn = std::make_unique<std::ifstream>(pathIn);
        streamIn = fileIn.get();
    }

    std::string pathOut = result["output"].as<std::string>();
    std::unique_ptr<std::ofstream> fileOut;
    std::ostream *streamOut = &std::cout;
    if (!pathOut.empty()) {
        fileOut = std::make_unique<std::ofstream>(pathOut);
        streamOut = fileOut.get();
    }

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return EXIT_SUCCESS;
    }

    if (result.count("list")) {
        Registry::printPlayers(std::cout);
        return EXIT_SUCCESS;
    }

    if (result.count("version")) {
        version();
        return EXIT_SUCCESS;
    }

    Game::run(names, *streamIn, *streamOut);
}

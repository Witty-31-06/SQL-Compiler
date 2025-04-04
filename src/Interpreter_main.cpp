#include "SQL_Intepreter.h"
#include <iostream>
#include <unordered_map>
#include <vector>
void printHelp(const std::string& progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  -v, --verbose <1|2>  Set verbosity level (default: 1)\n"
              << "  -h, --help           Show this help message\n";
}

int main(int argc, char* argv[]) {
    int verbosity = 1; // Default verbosity level

    std::unordered_map<std::string, std::string> args;
    std::vector<std::string> validFlags = {"-v", "--verbose", "-h", "--help"};

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (std::find(validFlags.begin(), validFlags.end(), arg) != validFlags.end()) {
            if (arg == "-h" || arg == "--help") {
                printHelp(argv[0]);
                return 0;
            }
            if ((arg == "-v" || arg == "--verbose") && i + 1 < argc) {
                args[arg] = argv[++i]; // Store the next argument as the verbosity level
            }
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            printHelp(argv[0]);
            return 1;
        }
    }

    // Process verbosity level
    if (args.find("-v") != args.end() || args.find("--verbose") != args.end()) {
        std::string value = args.find("-v") != args.end() ? args["-v"] : args["--verbose"];
        try {
            verbosity = std::stoi(value);
            if (verbosity != 1 && verbosity != 2) {
                throw std::invalid_argument("Invalid verbosity level.");
            }
        } catch (...) {
            std::cerr << "Error: Verbosity must be 1 or 2.\n";
            return 1;
        }
    }

    // Initialize SQL Interpreter with verbosity
    SQLInterpreter sqlInterpreter(verbosity);
    sqlInterpreter.init_interpreter();

    return 0;
}
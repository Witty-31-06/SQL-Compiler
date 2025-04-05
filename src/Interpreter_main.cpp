#include "SQL_Intepreter.h"
#include <iostream>
#include <unordered_map>
#include <vector>

/**
 * @brief Prints the help message for the SQL interpreter program.
 *
 * Displays the usage instructions and available command-line options to the standard output.
 * This function is called when the user requests help via the -h or --help flag or when an error occurs.
 *
 * @param progName The name of the program (typically argv[0]).
 */
void printHelp(const std::string& progName) {
    std::cout << "Usage: " << progName << " [options]\n"
              << "Options:\n"
              << "  -v, --verbose <1|2>  Set verbosity level (default: 1)\n"
              << "  -h, --help           Show this help message\n";
}

/**
 * @brief Main entry point for the SQL interpreter program.
 *
 * Parses command-line arguments to configure the verbosity level and initializes the SQL interpreter.
 * Supports options for verbosity (-v or --verbose) and help (-h or --help). If invalid arguments
 * or verbosity levels are provided, an error message is displayed, and the program exits with a
 * non-zero status.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Exit status (0 for success, 1 for failure).
 */
int main(int argc, char* argv[]) {
    int verbosity = 1; // Default verbosity level

    std::unordered_map<std::string, std::string> args; ///< Stores parsed command-line arguments.
    std::vector<std::string> validFlags = {"-v", "--verbose", "-h", "--help"}; ///< List of valid flags.

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
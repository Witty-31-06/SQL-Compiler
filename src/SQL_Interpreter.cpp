#include "SQL_Intepreter.h"
#include <iostream>
using namespace std;

/**
 * @file SQL_Interpreter.cpp
 * @brief Implementation of the SQLInterpreter class for interpreting SQL queries.
 *
 * This file contains the definitions of the SQLInterpreter class methods, which integrate a lexer
 * and parser to process and interpret SQL queries interactively based on a verbosity level.
 */

/**
 * @brief Constructs an SQLInterpreter with a specified verbosity level.
 *
 * Initializes the lexer and parser (with a grammar file "grammar.json") and sets the verbosity level.
 *
 * @param verbosity The verbosity level (e.g., 1 for minimal, 2 for detailed).
 */
SQLInterpreter::SQLInterpreter(int verbosity) : lexer(), parser("grammar.json")
{
    this->verbosity = verbosity;
}

/**
 * @brief Prints the list of tokens generated by the lexer.
 *
 * Iterates through the token vector and outputs each token's type and lexeme to the console.
 *
 * @param tokens The vector of Token objects to print.
 */
void SQLInterpreter::printTokens(vector<Token> tokens)
{
    for (const auto& token : tokens)
    {
        cout << token.getType() << " : " << token.getLexeme() << endl;
    }
}

/**
 * @brief Converts a string to lowercase.
 *
 * Uses std::transform with std::tolower to convert all characters in the string to lowercase.
 *
 * @param str The string to convert (modified in place).
 */
void SQLInterpreter::toLowercase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}

/**
 * @brief Initializes and runs the SQL interpreter.
 *
 * Runs an interactive loop that:
 * - Prints parser details (grammar, first/follow sets, LL1 table) if verbosity is 2.
 * - Prompts for SQL input, tokenizes it, and parses it until "exit" is entered.
 * - Displays tokens if verbosity is 2.
 */
void SQLInterpreter::init_interpreter()
{
    if(verbosity == 2){
        parser.printGrammar();
        parser.printFirstAndFollow();
        parser.printLL1Table();
    }
    while (true)
    {
        cout << "SQL> ";
        string input, fullQuery;

        while (true)
        {
            getline(cin, input);
            toLowercase(input);
            if (input == "exit" || input == "EXIT")
                return;

            fullQuery += input + " ";

            if (input.find(';') != string::npos)
                break;
            cout << "   > ";
        }

        fullQuery = trim(fullQuery);

        if (fullQuery.empty())
            continue;

        vector<Token> tokens = lexer.tokenize(fullQuery);
        if(verbosity == 2) {
            printTokens(tokens);
        }
        parser.parse(tokens);
    }
}

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * Finds the first and last non-whitespace characters and returns the substring between them.
 *
 * @param str The string to trim.
 * @return The trimmed string, or an empty string if str is all whitespace.
 */
string SQLInterpreter::trim(const string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
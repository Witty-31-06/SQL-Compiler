#ifndef __PARSER_H
#define __PARSER_H

#include <bits/stdc++.h>
#include <memory>
#include "Token.h"
using namespace std;

/**
 * @file parser.h
 * @brief Header file for the Parser class and related symbol classes for LL1 parsing of SQL queries.
 *
 * This file defines the Symbol, NonTerminal, Terminal, and Parser classes, which together implement
 * an LL1 parser for SQL queries using a grammar loaded from a file.
 */

/**
 * @class Symbol
 * @brief Base class representing a symbol in the grammar (terminal or non-terminal).
 *
 * The Symbol class encapsulates a string representation of a grammar symbol and a flag indicating
 * whether it is a terminal or non-terminal symbol.
 */
class Symbol {
    protected:
        string symbol; ///< The string representation of the symbol.
        bool isTerminal; ///< True if the symbol is a terminal, false if it is a non-terminal.

    public:
        /**
         * @brief Constructs a Symbol with a given name and terminal status.
         * @param symbol The string representation of the symbol.
         * @param isTerminal True if the symbol is a terminal, false otherwise.
         */
        Symbol(string symbol, bool isTerminal);

        /**
         * @brief Gets the string representation of the symbol.
         * @return The symbol string.
         */
        string getSymbol() const;

        /**
         * @brief Checks if the symbol is a terminal.
         * @return True if the symbol is a terminal, false otherwise.
         */
        bool getIsTerminal() const;

        /**
         * @brief Compares this symbol with another for equality.
         * @param other The other Symbol to compare with.
         * @return True if the symbols are equal, false otherwise.
         */
        bool operator==(const Symbol &other) const;
};

/**
 * @struct SymbolHash
 * @brief Hash function object for Symbol objects.
 *
 * Provides a hash function for use with unordered containers that store Symbol objects.
 */
struct SymbolHash {
    /**
     * @brief Computes the hash value of a Symbol.
     * @param s The Symbol to hash.
     * @return The hash value of the symbol.
     */
    size_t operator()(const Symbol &s) const;
};

/**
 * @class NonTerminal
 * @brief Derived class representing a non-terminal symbol in the grammar.
 *
 * Inherits from Symbol and specifies that the symbol is a non-terminal.
 */
class NonTerminal : public Symbol {
    public:
        /**
         * @brief Constructs a NonTerminal with a given name.
         * @param symbol The string representation of the non-terminal.
         */
        NonTerminal(string symbol);

        /**
         * @brief Compares this non-terminal with another for equality.
         * @param other The other NonTerminal to compare with.
         * @return True if the non-terminals are equal, false otherwise.
         */
        bool operator==(const NonTerminal &other) const;
};

/**
 * @class Terminal
 * @brief Derived class representing a terminal symbol in the grammar.
 *
 * Inherits from Symbol and specifies that the symbol is a terminal.
 */
class Terminal : public Symbol {
    public:
        /**
         * @brief Constructs a Terminal with a given name.
         * @param symbol The string representation of the terminal.
         */
        Terminal(string symbol);

        /**
         * @brief Compares this terminal with another for equality.
         * @param other The other Terminal to compare with.
         * @return True if the terminals are equal, false otherwise.
         */
        bool operator==(const Terminal &other) const;
};

/**
 * @class Parser
 * @brief A class implementing an LL1 parser for SQL queries.
 *
 * The Parser class loads a grammar from a file, computes the FIRST and FOLLOW sets, constructs an
 * LL1 parsing table, and uses it to parse a sequence of tokens generated from an SQL query.
 */
class Parser {
    private:
        unordered_map<NonTerminal, vector<vector<shared_ptr<Symbol>>>, SymbolHash> grammar; ///< Grammar rules mapping non-terminals to productions.
        shared_ptr<NonTerminal> start; ///< The start symbol of the grammar.
        unordered_map<NonTerminal, unordered_set<Terminal, SymbolHash>, SymbolHash> first; ///< FIRST sets for non-terminals.
        unordered_map<NonTerminal, unordered_set<Terminal, SymbolHash>, SymbolHash> follow; ///< FOLLOW sets for non-terminals.
        unordered_map<NonTerminal, unordered_map<Terminal, vector<shared_ptr<Symbol>>, SymbolHash>, SymbolHash> ll1Table; ///< LL1 parsing table.

    public:
        /**
         * @brief Constructs a Parser by loading a grammar from a file.
         * @param filename The path to the grammar file (e.g., JSON format).
         */
        Parser(const string& filename);

        /**
         * @brief Prints the loaded grammar rules.
         */
        void printGrammar() const;

        /**
         * @brief Applies left factoring to the grammar to ensure LL1 compatibility.
         */
        void leftFactorGrammar();

        /**
         * @brief Computes the FIRST sets for all non-terminals in the grammar.
         */
        void computeFirst();

        /**
         * @brief Computes the FOLLOW sets for all non-terminals in the grammar.
         */
        void computeFollow();

        /**
         * @brief Prints the computed FIRST and FOLLOW sets.
         */
        void printFirstAndFollow() const;

        /**
         * @brief Computes the LL1 parsing table based on the grammar and FIRST/FOLLOW sets.
         */
        void computeLL1Table();

        /**
         * @brief Prints the constructed LL1 parsing table.
         */
        void printLL1Table() const;

        /**
         * @brief Parses a sequence of tokens using the LL1 parsing table.
         * @param tokens The vector of Token objects to parse.
         * @return True if the parse succeeds, false otherwise.
         */
        bool parse(vector<Token> tokens);
};

#endif
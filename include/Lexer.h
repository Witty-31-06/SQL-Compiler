#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <regex>
#include "Token.h"

using namespace std;

/**
 * @file Lexer.h
 * @brief Header file for the Lexer class, responsible for tokenizing SQL query strings.
 *
 * This file defines the Lexer class, which processes input strings and converts them into a sequence
 * of tokens for use in an LL1 parser for SQL queries.
 */

/**
 * @class Lexer
 * @brief A class that tokenizes SQL query strings into a vector of Token objects.
 *
 * The Lexer class takes an input string (e.g., an SQL query) and breaks it down into individual tokens,
 * such as keywords, identifiers, operators, and literals. It uses helper methods to classify each token
 * based on predefined rules and stores the results in a vector.
 */
class Lexer
{
private:
    string filename; ///< Name of the file being tokenized (if applicable).
    vector<Token> tokens; ///< Vector storing the generated tokens.

    /**
     * @brief Removes leading and trailing spaces from a string.
     * @param t The string to process.
     * @return The trimmed string with spaces removed.
     */
    static string removeSpaces(string &t);

    /**
     * @brief Checks if a string is an SQL keyword.
     * @param str The string to check.
     * @return True if the string is a keyword, false otherwise.
     */
    static bool isKeyword(const string &str);

    /**
     * @brief Checks if a string is a valid SQL identifier.
     * @param str The string to check.
     * @return True if the string is an identifier, false otherwise.
     */
    static bool isIdentifier(const string &str);

    /**
     * @brief Checks if a string is an SQL operator.
     * @param str The string to check.
     * @return True if the string is an operator, false otherwise.
     */
    bool isOperator(const string &str);

    /**
     * @brief Checks if a string is a numeric literal.
     * @param str The string to check.
     * @return True if the string is a numeric literal, false otherwise.
     */
    bool isNumericLiteral(const string &str);

    /**
     * @brief Checks if a string is a string literal.
     * @param str The string to check.
     * @return True if the string is a string literal, false otherwise.
     */
    bool isStringLiteral(const string &str);

    /**
     * @brief Checks if a string is a punctuation mark.
     * @param str The string to check.
     * @return True if the string is punctuation, false otherwise.
     */
    bool isPunctuation(const string &str);

    /**
     * @brief Checks if a string matches all token classification rules (for debugging or special cases).
     * @param str The string to check.
     * @return True if the string matches all rules, false otherwise.
     */
    bool isAll(const string &str);

    /**
     * @brief Classifies a string into a specific Token type.
     * @param str The string to classify.
     * @return A Token object representing the classified string.
     */
    Token classifyToken(const string &str);

public:
    /**
     * @brief Default constructor for the Lexer class.
     *
     * Initializes an empty Lexer object with no tokens and an empty filename.
     */
    Lexer();

    /**
     * @brief Tokenizes an input string into a vector of Token objects.
     *
     * Processes the input string (e.g., an SQL query) and generates a sequence of tokens by identifying
     * keywords, identifiers, operators, literals, and punctuation.
     *
     * @param input The input string to tokenize.
     * @return A vector of Token objects representing the tokenized input.
     */
    vector<Token> tokenize(const string &input);
};

#endif
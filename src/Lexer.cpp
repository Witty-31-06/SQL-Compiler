#include "Lexer.h"
#include <cctype>
#include <iostream>
using namespace std;

/**
 * @file Lexer.cpp
 * @brief Implementation of the Lexer class for tokenizing SQL query strings.
 *
 * This file contains the definitions of the Lexer class methods, which process input strings and
 * convert them into a sequence of Token objects for use in an LL1 parser for SQL queries.
 */

/**
 * @brief Default constructor for the Lexer class.
 *
 * Initializes an empty Lexer object. The filename and tokens vector are left empty by default.
 */
Lexer::Lexer() {}

/**
 * @brief Removes all spaces from a string.
 *
 * Iterates through the input string and builds a new string containing only non-space characters.
 *
 * @param t The string to process.
 * @return The string with all spaces removed.
 */
string Lexer::removeSpaces(string &t)
{
    string ans = "";
    for (auto c : t)
    {
        if (c != ' ')
        {
            ans += c;
        }
    }
    return ans;
}

/**
 * @brief Checks if a string is an SQL keyword.
 *
 * Uses a regular expression to match the string against a list of SQL keywords.
 *
 * @param str The string to check.
 * @return True if the string is a keyword, false otherwise.
 */
bool Lexer::isKeyword(const string &str)
{
    const string keywordRegex = "select|from|where|create|table|insert|primary|key|into|values|number|char|varchar";
    return regex_match(str, regex(keywordRegex));
}

/**
 * @brief Checks if a string is a valid SQL identifier.
 *
 * Uses a regular expression to match the string against the pattern for SQL identifiers
 * (starts with a letter, followed by letters, digits, or underscores).
 *
 * @param str The string to check.
 * @return True if the string is an identifier, false otherwise.
 */
bool Lexer::isIdentifier(const string &str)
{
    const string identifierRegex = "[a-zA-Z][a-zA-Z0-9_]*";
    return regex_match(str, regex(identifierRegex));
}

/**
 * @brief Checks if a string is an SQL operator.
 *
 * Uses a regular expression to match the string against a list of SQL operators.
 *
 * @param str The string to check.
 * @return True if the string is an operator, false otherwise.
 */
bool Lexer::isOperator(const string &str)
{
    const string operatorRegex = "=|>|<|between|like|in|\\.";
    return regex_match(str, regex(operatorRegex));
}

/**
 * @brief Checks if a string is a numeric literal.
 *
 * Uses a regular expression to match the string against the pattern for numeric literals
 * (non-zero starting digits or single zero).
 *
 * @param str The string to check.
 * @return True if the string is a numeric literal, false otherwise.
 */
bool Lexer::isNumericLiteral(const string &str)
{
    const string numericLiteralRegex = "[1-9]+[0-9]*|[0-9]";
    return regex_match(str, regex(numericLiteralRegex));
}

/**
 * @brief Checks if a string is a string literal.
 *
 * Uses a regular expression to match the string against the pattern for string literals
 * (text enclosed in single quotes).
 *
 * @param str The string to check.
 * @return True if the string is a string literal, false otherwise.
 */
bool Lexer::isStringLiteral(const string &str)
{
    const string stringLiteralRegex = "\\'[a-zA-Z]*\\'";
    return regex_match(str, regex(stringLiteralRegex));
}

/**
 * @brief Checks if a string represents the "all" wildcard (*).
 *
 * Uses a regular expression to match the string against the asterisk (*) character.
 *
 * @param str The string to check.
 * @return True if the string is "*", false otherwise.
 */
bool Lexer::isAll(const string &str)
{
    const string allRegex = "\\*";
    return regex_match(str, regex(allRegex));
}

/**
 * @brief Tokenizes an input string into a vector of Token objects.
 *
 * Processes the input SQL query string character by character, extracting tokens based on spaces
 * and special characters (e.g., parentheses, semicolon). Each extracted string is classified
 * and added to the token vector.
 *
 * @param query The input SQL query string to tokenize.
 * @return A vector of Token objects representing the tokenized input.
 */
vector<Token> Lexer::tokenize(const string& query) {
    vector<Token> tokens;
    string extractedString = "";

    for (size_t i = 0; i < query.length(); i++) {
        char c = query[i];

        // Check for special characters
        if (c == '(' || c == ')' || c == '<' || c == '>' || c == ';' || c == '.' || c == ',') {
            if (!extractedString.empty()) {
                tokens.push_back(classifyToken(extractedString));
                extractedString = "";
            }
            string specialChar(1, c);
            tokens.push_back(classifyToken(specialChar));
        }
        else if (isspace(c)) {
            if (!extractedString.empty()) {
                tokens.push_back(classifyToken(extractedString));
                extractedString = "";
            }
        }
        else {
            extractedString += c;
        }
    }

    // Handle last extracted token
    if (!extractedString.empty()) {
        tokens.push_back(classifyToken(extractedString));
    }

    return tokens;
}

/**
 * @brief Classifies a string into a specific Token type.
 *
 * Evaluates the input string against various token types (keyword, operator, identifier, etc.)
 * and returns a corresponding Token object. If the string cannot be classified, an error is
 * printed, and the program exits.
 *
 * @param token The string to classify.
 * @return A Token object representing the classified string.
 */
Token Lexer::classifyToken(const string& token) {
    if (isKeyword(token)) return Token("KEYWORD", token);
    if (isOperator(token)) return Token("OPERATOR", token);
    if (isIdentifier(token)) return Token("IDENTIFIER", token);
    if (isNumericLiteral(token)) return Token("NUMERIC LITERAL", token);
    if (isAll(token)) return Token("ALL", token);
    if (isStringLiteral(token)) return Token("STRING LITERAL", token);

    if (token == "(") return Token("OPEN_BRACKET", token);
    if (token == ")") return Token("CLOSE_BRACKET", token);
    if (token == ";") return Token("SEMICOLON", token);
    if (token == ",") return Token("COMMA", token);

    cerr << "Unidentified token: " << token << endl;
    exit(1);
}
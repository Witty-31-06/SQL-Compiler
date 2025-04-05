#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

/**
 * @file Token.h
 * @brief Header file for the Token class, representing a single token in an SQL query.
 *
 * This file defines the Token class, which encapsulates the type and lexeme of a token generated
 * during the lexical analysis of SQL queries.
 */

/**
 * @class Token
 * @brief A class representing a single token in an SQL query.
 *
 * The Token class stores the type (e.g., "KEYWORD", "IDENTIFIER") and lexeme (the actual string)
 * of a token, providing accessors to retrieve these values. It is used by the Lexer and Parser
 * in an LL1 SQL interpreter.
 */
class Token
{
    string lexeme; ///< The actual string value of the token (e.g., "SELECT", "table1").
    string type;   ///< The type or category of the token (e.g., "KEYWORD", "IDENTIFIER").

public:
    /**
     * @brief Constructs a Token with a specified type and lexeme.
     *
     * Initializes a Token object with the given type and lexeme values.
     *
     * @param t The type of the token (e.g., "KEYWORD", "OPERATOR").
     * @param l The lexeme of the token (the actual string it represents).
     */
    Token(string t, string l);

    /**
     * @brief Gets the lexeme of the token.
     *
     * Returns the actual string value of the token as it appeared in the input.
     *
     * @return The lexeme of the token.
     */
    string getLexeme() const;

    /**
     * @brief Gets the type of the token.
     *
     * Returns the category or classification of the token.
     *
     * @return The type of the token.
     */
    string getType() const;
};

#endif
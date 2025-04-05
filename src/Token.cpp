#include "Token.h"

/**
 * @file Token.cpp
 * @brief Implementation of the Token class for representing tokens in an SQL query.
 *
 * This file contains the definitions of the Token class methods, which manage the type and lexeme
 * of tokens generated during the lexical analysis of SQL queries.
 */

/**
 * @brief Constructs a Token with a specified type and lexeme.
 *
 * Initializes the Token object using member initialization to set the type and lexeme.
 *
 * @param t The type of the token (e.g., "KEYWORD", "OPERATOR").
 * @param l The lexeme of the token (the actual string it represents).
 */
Token::Token(string t, string l) : type(t), lexeme(l) {}

/**
 * @brief Gets the lexeme of the token.
 *
 * Returns the stored lexeme string as a const value.
 *
 * @return The lexeme of the token.
 */
string Token::getLexeme() const
{
    return lexeme;
}

/**
 * @brief Gets the type of the token.
 *
 * Returns the stored type string as a const value.
 *
 * @return The type of the token.
 */
string Token::getType() const
{
    return type;
}
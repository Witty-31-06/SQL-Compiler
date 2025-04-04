#include "Token.h"

Token::Token(string t, string l) : type(t), lexeme(l) {}

string Token::getLexeme() const
{
    return lexeme;
}

string Token::getType() const
{
    return type;
}
#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include <fstream>
#include "Token.h"

using namespace std;

class Lexer
{
private:
    string filename;
    vector<Token> tokens;

    string removeSpaces(string &t);
    bool isKeyword(const string &str);
    bool isIdentifier(const string &str);
    bool isOperator(const string &str);
    bool isNumericLiteral(const string &str);
    bool isStringLiteral(const string &str);
    bool isPunctuation(const string &str);
    bool isAll(const string &str);
    Token classifyToken(const string &str);

public:
    Lexer();
    vector<Token> tokenize(const string &);
};

#endif
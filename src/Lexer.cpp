#include "Lexer.h"
#include <sstream>
#include <cctype>
#include <iostream>
using namespace std;
Lexer::Lexer(){}
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

bool Lexer::isKeyword(const string &str)
{
    const string keywordRegex = "select|from|where|create|table|insert|primary|key|into|values|number|char|varchar";
    return regex_match(str, regex(keywordRegex));
}

bool Lexer::isIdentifier(const string &str)
{
    // cout<<str<<endl;
    const string identifierRegex = "[a-zA-Z][a-zA-Z0-9_]*";
    return regex_match(str, regex(identifierRegex));
}

bool Lexer::isOperator(const string &str)
{
    const string operatorRegex = "=|>|<|between|like|in|\\.";
    return regex_match(str, regex(operatorRegex));
}

bool Lexer::isNumericLiteral(const string &str)
{
    const string numericLiteralRegex = "[1-9]+[0-9]*|[0-9]";
    return regex_match(str, regex(numericLiteralRegex));
}

bool Lexer::isStringLiteral(const string &str)
{
    const string stringLiteralRegex = "\\'[a-zA-Z]*\\'";
    return regex_match(str, regex(stringLiteralRegex));
}


bool Lexer::isAll(const string &str)
{
    const string allRegex = "\\*";
    return regex_match(str, regex(allRegex));
}




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
#ifndef TOKEN_H
#define TOKEN_H

#include <string>
using namespace std;

class Token
{
    string lexeme;
    string type;

public:
    Token(string t, string l);
    string getLexeme() const;
    string getType() const;
};

#endif
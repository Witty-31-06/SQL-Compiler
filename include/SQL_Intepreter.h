#ifndef __SQL_INTERPRETER_H__
#define __SQL_INTERPRETER_H__

#include <iostream>
#include "Lexer.h"
#include "parser.h"
class SQLInterpreter
{
    private:
        Lexer lexer;
        int verbosity;
        void printTokens(vector<Token> tokens);
        Parser parser;
        string trim(const string &str);
        void toLowercase(std::string& str);
    public:
        SQLInterpreter(int verbosity);
        ~SQLInterpreter() = default;

        void init_interpreter();
};
#endif
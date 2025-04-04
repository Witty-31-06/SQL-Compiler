#include "SQL_Intepreter.h"
#include <iostream>
using namespace std;
SQLInterpreter::SQLInterpreter() : lexer(), parser("grammar.json")
{
    std::cout << "SQLInterpreter constructor called" << std::endl;
}
void SQLInterpreter::printTokens(vector<Token> tokens)
{
    for (const auto& token : tokens)
    {
        cout << token.getType() << " : " << token.getLexeme() << endl;
    }
}
void SQLInterpreter::toLowercase(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}
void SQLInterpreter::init_interpreter()
{
    while (true)
    {
        cout << "SQL> ";
        string input, fullQuery;
        
        while (true)
        {
            getline(cin, input);
            toLowercase(input);
            if (input == "exit" || input == "EXIT")
                return;

            fullQuery += input + " ";

            if (input.find(';') != string::npos)
                break;
            cout << "   > ";
        }

        fullQuery = trim(fullQuery);

        if (fullQuery.empty())
            continue;

        vector<Token> tokens = lexer.tokenize(fullQuery);
        printTokens(tokens);
        parser.parse(tokens);
    }
}
string SQLInterpreter::trim(const string& str)
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
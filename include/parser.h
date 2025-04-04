#ifndef __PARSER_H
#define __PARSER_H

#include <bits/stdc++.h>
#include <memory>
#include "Token.h"
using namespace std;

class Symbol {
    protected:
        string symbol;
        bool isTerminal;

    public: 
        Symbol(string symbol, bool isTerminal);
        string getSymbol() const;
        bool getIsTerminal() const;
        bool operator==(const Symbol &other) const;
};

struct SymbolHash {
    size_t operator()(const Symbol &s) const;
};

class NonTerminal : public Symbol {
    public:
        NonTerminal(string symbol);
        bool operator==(const NonTerminal &other) const;
};

class Terminal : public Symbol {
    public:
        Terminal(string symbol);
        bool operator==(const Terminal &other) const;
};

class Parser {
    private:
        unordered_map<NonTerminal, vector<vector<shared_ptr<Symbol>>>, SymbolHash> grammar;
        shared_ptr<NonTerminal> start;
        unordered_map<NonTerminal, unordered_set<Terminal, SymbolHash>, SymbolHash> first;
        unordered_map<NonTerminal, unordered_set<Terminal, SymbolHash>, SymbolHash> follow;
        unordered_map<NonTerminal, unordered_map<Terminal, vector<shared_ptr<Symbol>>, SymbolHash>, SymbolHash> ll1Table;
        public:
        Parser(const string& filename);
        void printGrammar() const;
        void leftFactorGrammar();
        void computeFirst();
        void computeFollow();
        void printFirstAndFollow() const;
        void computeLL1Table();
        void printLL1Table() const;
        bool parse(vector<Token> tokens);
};


#endif

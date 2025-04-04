#include "parser.h"
#include <iostream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <memory>
#include <iomanip>
#include <tabulate/table.hpp>

using namespace tabulate;
using json = nlohmann::json;
using namespace std;

// Constructor and methods for Symbol
Symbol::Symbol(string s, bool t) : symbol(s), isTerminal(t) {}

string Symbol::getSymbol() const { return symbol; }
bool Symbol::getIsTerminal() const { return isTerminal; }

bool Symbol::operator==(const Symbol& other) const {
    return symbol == other.symbol && isTerminal == other.isTerminal;
}

bool NonTerminal::operator==(const NonTerminal& other) const {
    return this->getSymbol() == other.getSymbol();
}

bool Terminal::operator==(const Terminal& other) const {
    return this->getSymbol() == other.getSymbol();
}

NonTerminal::NonTerminal(string symbol) : Symbol(symbol, false) {}
Terminal::Terminal(string symbol) : Symbol(symbol, true) {}

size_t SymbolHash::operator()(const Symbol& s) const {
    return std::hash<string>()(s.getSymbol());
}

Parser::Parser(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return;
    }

    json j;
    file >> j;

    string startSymbol = j["start-symbol"].get<string>();
    start = make_shared<NonTerminal>(startSymbol);

    unordered_set<string> nonTerminals;
    for (const auto& nt : j["non-terminals"]) {
        nonTerminals.insert(nt.get<string>());
    }

    for (const auto& prod : j["productions"]) {
        string lhs = prod["lhs"].get<string>();
        NonTerminal nonTerminal(lhs);

        vector<shared_ptr<Symbol>> rhsSymbols;
        for (const string& rhs : prod["rhs"]) {
            if (nonTerminals.find(rhs) != nonTerminals.end()) {  // If it's a NonTerminal
                rhsSymbols.push_back(make_shared<NonTerminal>(rhs));
            } else {  // Otherwise, it's a Terminal
                rhsSymbols.push_back(make_shared<Terminal>(rhs));
            }
        }

        grammar[nonTerminal].push_back(rhsSymbols);
    }

    file.close();
    leftFactorGrammar();
    computeFirst();
    computeFollow();
    computeLL1Table();

}

void Parser::printGrammar() const {
    cout << "Start Symbol: " << start->getSymbol() << endl;
    cout << "Grammar Rules:\n";

    for (const auto& rule : grammar) {
        cout << rule.first.getSymbol() << " -> ";
        for (const auto& rhs : rule.second) {
            for (const auto& sym : rhs) {
                cout << sym->getSymbol() << " ";
            }
            cout << "| ";
        }
        cout << endl;
    }
    cout<<string(50,'-')<<endl;
}


void Parser::leftFactorGrammar() {
    unordered_map<NonTerminal, vector<vector<shared_ptr<Symbol>>>, SymbolHash> newGrammar;

    for (const auto& rule : grammar) {
        NonTerminal A = rule.first;
        unordered_map<string, vector<vector<shared_ptr<Symbol>>>> groupedRules;
        vector<vector<shared_ptr<Symbol>>> remaining;

        for (const auto& rhs : rule.second) {
            if (!rhs.empty()) {
                groupedRules[rhs[0]->getSymbol()].push_back(rhs);
            } else {
                remaining.push_back({make_shared<Terminal>("#")});  // Preserve epsilon
            }
        }

        for (const auto& [prefix, productions] : groupedRules) {
            if (productions.size() > 1) {  // Left factoring needed
                string newName = A.getSymbol() + "'";
                NonTerminal A_prime(newName);
                vector<vector<shared_ptr<Symbol>>> newAProductions;

                // Find the longest common prefix
                vector<shared_ptr<Symbol>> commonPrefix = productions[0];
                for (const auto& rhs : productions) {
                    size_t i = 0;
                    while (i < commonPrefix.size() && i < rhs.size() && commonPrefix[i]->getSymbol() == rhs[i]->getSymbol()) {
                        i++;
                    }
                    commonPrefix.resize(i);  // Reduce to the actual common prefix
                }

                // Replace original rule with factored version
                vector<shared_ptr<Symbol>> factoredProduction = commonPrefix;
                factoredProduction.push_back(make_shared<NonTerminal>(newName));
                newGrammar[A].push_back(factoredProduction);

                // Create new rules for A'
                for (const auto& rhs : productions) {
                    vector<shared_ptr<Symbol>> suffix(rhs.begin() + commonPrefix.size(), rhs.end());
                    if (suffix.empty()) {
                        newAProductions.push_back({make_shared<Terminal>("#")});  // A' -> ε
                    } else {
                        newAProductions.push_back(suffix);
                    }
                }
                newGrammar[A_prime] = newAProductions;
            } else {
                remaining.insert(remaining.end(), productions.begin(), productions.end());
            }
        }

        if (!remaining.empty()) {

            for(auto& r:remaining){
                newGrammar[A].push_back(r);

            }
        }
    }

    grammar = newGrammar;
}

void Parser::computeFirst() {
    bool changed;
    do {
        changed = false;

        for (const auto& rule : grammar) {
            NonTerminal A = rule.first;

            for (const auto& rhs : rule.second) {
                bool hasEpsilon = true;

                for (const auto& sym : rhs) {
                    if (sym->getIsTerminal()) {
                        if (first[A].insert(Terminal(sym->getSymbol())).second) {
                            changed = true;
                        }
                        hasEpsilon = false;
                        break;
                    } else {
                        NonTerminal B(sym->getSymbol());
                        for (const auto& term : first[B]) {
                            if (term.getSymbol() != "#" && first[A].insert(term).second) {
                                changed = true;
                            }
                        }
                        if (first[B].count(Terminal("#")) == 0) {
                            hasEpsilon = false;
                            break;
                        }
                    }
                }

                if (hasEpsilon) {
                    if (first[A].insert(Terminal("#")).second) {
                        changed = true;
                    }
                }
            }
        }
    } while (changed);
}



void Parser::computeFollow() {
    // Initialize FOLLOW set of start symbol with "$"
    follow[*start].insert(Terminal("$"));

    bool changed;
    do {
        changed = false;

        for (const auto& rule : grammar) {
            NonTerminal A = rule.first;

            for (const auto& rhs : rule.second) {
                size_t n = rhs.size();

                for (size_t i = 0; i < n; ++i) {
                    if (!rhs[i]->getIsTerminal()) {
                        NonTerminal B(rhs[i]->getSymbol());

                        // Case 1: A -> α B β (Add FIRST(β) - {ε} to FOLLOW(B))
                        bool addFollowA = true;
                        for (size_t j = i + 1; j < n; ++j) {
                            if (rhs[j]->getIsTerminal()) {
                                // Terminal found, add it to FOLLOW(B)
                                if (follow[B].insert(Terminal(rhs[j]->getSymbol())).second) {
                                    changed = true;
                                }
                                addFollowA = false;
                                break;
                            } else {
                                // Non-terminal found, add its FIRST set (excluding ε) to FOLLOW(B)
                                NonTerminal C(rhs[j]->getSymbol());
                                for (const auto& term : first[C]) {
                                    if (term.getSymbol() != "#" && follow[B].insert(term).second) {
                                        changed = true;
                                    }
                                }

                                // If FIRST(C) does not contain ε, stop here
                                if (first[C].count(Terminal("#")) == 0) {
                                    addFollowA = false;
                                    break;
                                }
                            }
                        }

                        // Case 2: A -> α B (or if β contains ε), then FOLLOW(A) ⊆ FOLLOW(B)
                        if (addFollowA) {
                            for (const auto& term : follow[A]) {
                                if (follow[B].insert(term).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (changed);
}

void Parser::printFirstAndFollow() const {
    cout << "FIRST Sets:\n";
    for (const auto& entry : first) {
        cout << "FIRST(" << entry.first.getSymbol() << ") = { ";
        for (const auto& sym : entry.second) {
            cout << sym.getSymbol() << " ";
        }
        cout << "}\n";
    }

    cout << "\nFOLLOW Sets:\n";
    for (const auto& entry : follow) {
        cout << "FOLLOW(" << entry.first.getSymbol() << ") = { ";
        for (const auto& sym : entry.second) {
            cout << sym.getSymbol() << " ";
        }
        cout << "}\n";
    }
    cout<<string(50,'-')<<endl;

}

void Parser::computeLL1Table() {
    for (const auto& rule : grammar) {
        NonTerminal A = rule.first;  // Left-hand side of the production
        // cout;
        for (const auto& rhs : rule.second) {  // Right-hand side options
            unordered_set<Terminal, SymbolHash> firstSet;
            bool hasEpsilon = true;  // Assume epsilon is in FIRST(rhs)

            for (const auto& sym : rhs) {
                if (sym->getSymbol() == "#") {
                    continue;
                }  
                else if (sym->getIsTerminal()) {  
                    // If the symbol is a terminal, add it and stop processing
                    firstSet.insert(Terminal(sym->getSymbol()));
                    hasEpsilon = false;
                    break;  // No need to check further symbols
                } else {  
                    // If it's a non-terminal, add its FIRST set
                    NonTerminal B(sym->getSymbol());
                    bool foundEpsilon = false;

                    for (const auto& term : first[B]) {
                        if (term.getSymbol() != "#") {  // Exclude epsilon
                            firstSet.insert(term);
                        } else {
                            foundEpsilon = true;  // This non-terminal has epsilon
                        }
                    }

                    if (!foundEpsilon) {
                        hasEpsilon = false;
                        break;  // Stop if no epsilon is found in FIRST(B)
                    }
                }
            }

            // Step 1: Add rule to LL(1) table using FIRST(rhs)
            for (const auto& t : firstSet) {
                if (t.getSymbol() != "#") {  
                    if (ll1Table[A].count(t) == 0) { 
                        ll1Table[A][t] = rhs;
                    } else {
                        cerr << "Error: Conflict in LL(1) table for " << A.getSymbol() << " with terminal " << t.getSymbol() << endl;
                    }
                }

            }

            // Step 2: If FIRST(rhs) contains epsilon, use FOLLOW(A)
            if (hasEpsilon) {
                for (const auto& t : follow[A]) {
                    if (ll1Table[A].count(t) == 0) {  // Avoid overwriting existing entries
                        ll1Table[A][t] = rhs;
                    }
                    else {
                        cerr << "Error: Conflict in LL(1) table for " << A.getSymbol() << " with terminal " << t.getSymbol() << endl;
                    }
                }
            }
        }
    }
}

void Parser::printLL1Table() const {
    cout << "\nLL(1) Parsing Table:\n";
    for (const auto& row : ll1Table) {
        cout << row.first.getSymbol() << " : ";
        for (const auto& col : row.second) {
            cout << "[" << col.first.getSymbol() << " -> ";
            for (const auto& sym : col.second) {
                cout << sym->getSymbol() << " ";
            }
            cout << "] ";
        }
        cout << endl;
    }
    cout<<string(50,'-')<<endl;

}

bool Parser::parse(vector<Token> tokens) {
    using namespace tabulate;

    tokens.push_back(Token("$", "$")); 
    stack<shared_ptr<Symbol>> parseStack;
    parseStack.push(make_shared<Terminal>("$"));
    parseStack.push(start);
    size_t tokenIndex = 0;
    Token currentToken = tokens[tokenIndex];

    Table traceTable;
    traceTable.add_row({"Stack", "Input", "Action"});
    traceTable[0].format()
        .font_style({FontStyle::bold})
        .font_align(FontAlign::center);

    while (!parseStack.empty()) {
        stack<shared_ptr<Symbol>> tempStack = parseStack;
        vector<string> stackContents;
        while (!tempStack.empty()) {
            stackContents.push_back(tempStack.top()->getSymbol());
            tempStack.pop();
        }

        string stackStr;
        for (auto it = stackContents.rbegin(); it != stackContents.rend(); ++it) {
            stackStr += *it + " ";
        }

        string inputStr = currentToken.getLexeme();
        string actionStr;

        shared_ptr<Symbol> top = parseStack.top();
        parseStack.pop();
        string topSymbol = top->getSymbol();
        string tokenType = currentToken.getType();
        string tokenLexeme = currentToken.getLexeme();

        if (tokenType == "NUMERIC LITERAL") tokenType = "number";
        else if (tokenType == "STRING LITERAL") tokenType = "string";
        else if (tokenType == "IDENTIFIER") tokenType = "identifier";

        if (top->getIsTerminal()) {
            if (topSymbol == tokenType || topSymbol == tokenLexeme) {
                actionStr = "Matched '" + tokenLexeme + "'";
                tokenIndex++;
                if (tokenIndex < tokens.size()) {
                    currentToken = tokens[tokenIndex];
                } else {
                    currentToken = Token("$", "$");
                }
            } else {
                cerr << "\nSyntax Error: Expected '" << topSymbol << "' but found '" << tokenLexeme << "'\n";
                return false;
            }
        } else {
            NonTerminal nonTerminal(topSymbol);
            auto &row = ll1Table[nonTerminal];

            if (row.find(Terminal(tokenLexeme)) != row.end()) {
                vector<shared_ptr<Symbol>> rule = row[Terminal(tokenLexeme)];
                actionStr = "Expand " + nonTerminal.getSymbol() + " → ";
                for (const auto &sym : rule) actionStr += sym->getSymbol() + " ";

                for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                    if ((*it)->getSymbol() != "#") parseStack.push(*it);
                }

            } else if (row.find(Terminal(tokenType)) != row.end()) {
                vector<shared_ptr<Symbol>> rule = row[Terminal(tokenType)];
                actionStr = "Expand " + nonTerminal.getSymbol() + " → ";
                for (const auto &sym : rule) actionStr += sym->getSymbol() + " ";

                for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                    if ((*it)->getSymbol() != "#") parseStack.push(*it);
                }

            } else {
                cerr << "\nSyntax Error: Unexpected token '(" << tokenLexeme << "," << tokenType << ")'\n";
                return false;
            }
        }

        auto &new_row = traceTable.add_row({stackStr, inputStr, actionStr});
    }

    if (tokenIndex < tokens.size()) {
        cerr << "\nSyntax Error: Extra input found after parsing\n";
        cout << tokenIndex << " " << tokens.size() << endl;
        cout << tokens[tokenIndex].getLexeme() << endl;
        return false;
    }

    cout << traceTable << endl;

    cout << "\nParsing successful!" << endl;
    return true;
}


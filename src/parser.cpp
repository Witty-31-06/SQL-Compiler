#include "parser.h"
#include <iostream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <memory>
#include <tabulate/table.hpp>

using json = nlohmann::json;
using namespace tabulate;
using namespace std;

/**
 * @file parser.cpp
 * @brief Implementation of the Parser class and related symbol classes for LL1 parsing of SQL queries.
 *
 * This file contains the definitions of the Symbol, NonTerminal, Terminal, and Parser class methods,
 * which implement an LL1 parser for SQL queries by loading a grammar, computing FIRST and FOLLOW sets,
 * constructing an LL1 table, and parsing token sequences.
 */

/**
 * @brief Constructs a Symbol with a given name and terminal status.
 * @param s The string representation of the symbol.
 * @param t True if the symbol is a terminal, false otherwise.
 */
Symbol::Symbol(string s, bool t) : symbol(s), isTerminal(t) {}

/** @brief Gets the string representation of the symbol. @return The symbol string. */
string Symbol::getSymbol() const { return symbol; }

/** @brief Checks if the symbol is a terminal. @return True if terminal, false otherwise. */
bool Symbol::getIsTerminal() const { return isTerminal; }

/**
 * @brief Compares this symbol with another for equality.
 * @param other The other Symbol to compare with.
 * @return True if the symbols are equal, false otherwise.
 */
bool Symbol::operator==(const Symbol& other) const {
    return symbol == other.symbol && isTerminal == other.isTerminal;
}

/**
 * @brief Compares this non-terminal with another for equality.
 * @param other The other NonTerminal to compare with.
 * @return True if equal, false otherwise.
 */
bool NonTerminal::operator==(const NonTerminal& other) const {
    return this->getSymbol() == other.getSymbol();
}

/**
 * @brief Compares this terminal with another for equality.
 * @param other The other Terminal to compare with.
 * @return True if equal, false otherwise.
 */
bool Terminal::operator==(const Terminal& other) const {
    return this->getSymbol() == other.getSymbol();
}

/** @brief Constructs a NonTerminal with a given name. @param symbol The non-terminal name. */
NonTerminal::NonTerminal(string symbol) : Symbol(symbol, false) {}

/** @brief Constructs a Terminal with a given name. @param symbol The terminal name. */
Terminal::Terminal(string symbol) : Symbol(symbol, true) {}

/**
 * @brief Computes the hash value of a Symbol.
 * @param s The Symbol to hash.
 * @return The hash value based on the symbol string.
 */
size_t SymbolHash::operator()(const Symbol& s) const {
    return hash<string>()(s.getSymbol());
}

/**
 * @brief Constructs a Parser by loading a grammar from a JSON file.
 *
 * Initializes the grammar, sets the start symbol, and computes necessary parsing structures.
 * @param filename The path to the JSON grammar file.
 */
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
        for (const auto& rhs : prod["rhs"]) {
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


/**
 * @brief Prints the loaded grammar rules in a tabulated format.
 *
 * Uses the tabulate library to display the start symbol and production rules.
 */
void Parser::printGrammar() const {
    Table table;
    table.add_row({"Start Symbol", start->getSymbol()});
    table[0].format().font_color(Color::yellow).font_style({FontStyle::bold});

    table.add_row({"Grammar Rules", ""});
    table[1].format().font_color(Color::cyan).font_style({FontStyle::bold});


    for (const auto& rule : grammar) {
        string lhs = rule.first.getSymbol();
        string rhs;

        for (const auto& rhs_list : rule.second) {
            for (const auto& sym : rhs_list) {
                rhs += sym->getSymbol() + " ";
            }
            rhs += "| ";
        }
        table.add_row({lhs, rhs});
        table.column(0).format().font_color(Color::red).font_style({FontStyle::bold});
        table.column(1).format().font_color(Color::blue).font_style({FontStyle::bold});

    }

    cout << table << endl<<endl<<endl;
}

/**
 * @brief Applies left factoring to the grammar to ensure LL1 compatibility.
 *
 * Groups productions by common prefixes and introduces new non-terminals to eliminate ambiguity.
 */
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

/**
 * @brief Computes the FIRST sets for all non-terminals in the grammar.
 *
 * Iteratively calculates the set of terminals that can appear as the first symbol of strings derived from each non-terminal.
 */
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


/**
 * @brief Computes the FOLLOW sets for all non-terminals in the grammar.
 *
 * Iteratively calculates the set of terminals that can appear immediately after each non-terminal in a derivation.
 */
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


/**
 * @brief Prints the computed FIRST and FOLLOW sets in tabulated format.
 *
 * Displays each non-terminal’s FIRST and FOLLOW sets using the tabulate library.
 */
void Parser::printFirstAndFollow() const {
    // Table for FIRST sets
    Table firstTable;
    firstTable.add_row({"Non-Terminal", "FIRST Set"});
    firstTable[0].format().font_style({FontStyle::bold}).font_color(Color::green);

    for (const auto& entry : first) {
        string nt = entry.first.getSymbol();
        string symbols;
        for (const auto& sym : entry.second) {
            symbols += sym.getSymbol() + " ";
        }
        firstTable.add_row({nt, "{ " + symbols + "}"});
        firstTable.column(0).format().font_color(Color::red).font_style({FontStyle::bold});
        firstTable.column(1).format().font_color(Color::blue).font_style({FontStyle::bold});
    }

    // Table for FOLLOW sets
    Table followTable;
    followTable.add_row({"Non-Terminal", "FOLLOW Set"});
    followTable[0].format().font_style({FontStyle::bold}).font_color(Color::yellow);

    for (const auto& entry : follow) {
        string nt = entry.first.getSymbol();
        string symbols;
        for (const auto& sym : entry.second) {
            symbols += sym.getSymbol() + " ";
        }
        followTable.add_row({nt, "{ " + symbols + "}"});
        followTable.column(0).format().font_color(Color::red).font_style({FontStyle::bold});
        followTable.column(1).format().font_color(Color::blue).font_style({FontStyle::bold});
    }

    cout << "\nFIRST Sets:\n" << firstTable << "\n\n";
    cout << "FOLLOW Sets:\n" << followTable << "\n";
}

/**
 * @brief Computes the LL1 parsing table based on the grammar and FIRST/FOLLOW sets.
 *
 * Constructs the table by mapping non-terminals and terminals to productions, checking for conflicts.
 */
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

/**
 * @brief Prints the constructed LL1 parsing table in tabulated format.
 *
 * Displays the non-terminal, terminal, and production entries using the tabulate library.
 */
void Parser::printLL1Table() const {
    Table table;
    table.add_row({"Non-Terminal", "Terminal", "Production"});
    table[0].format().font_style({FontStyle::bold}).font_color(Color::cyan);

    for (const auto& row : ll1Table) {
        const string& nonTerminal = row.first.getSymbol();

        for (const auto& col : row.second) {
            string terminal = col.first.getSymbol();
            string production;

            for (const auto& sym : col.second) {
                production += sym->getSymbol() + " ";
            }

            table.add_row({nonTerminal, terminal, production});
            table.column(0).format().font_color(Color::red).font_style({FontStyle::bold});
            table.column(1).format().font_color(Color::blue).font_style({FontStyle::bold});
            table.column(2).format().font_color(Color::green).font_style({FontStyle::bold});

        }

    }

    cout << "\nLL(1) Parsing Table:\n" << table << endl <<endl;
}

/**
 * @brief Parses a sequence of tokens using the LL1 parsing table.
 *
 * Uses a stack-based approach to match tokens against the grammar, printing a trace table.
 * @param tokens The vector of Token objects to parse.
 * @return True if the parse succeeds, false otherwise.
 */
bool Parser::parse(vector<Token> tokens) {

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
        .font_align(FontAlign::center)
        .font_color(Color::blue);
    int rowIndex = 1;
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
        bool matched = false;
        bool error = false;
        if (top->getIsTerminal()) {
            if (topSymbol == tokenType || topSymbol == tokenLexeme) {
                actionStr = "Matched '" + tokenLexeme + "'";
                tokenIndex++;
                if (tokenIndex < tokens.size()) {
                    currentToken = tokens[tokenIndex];
                } else {
                    currentToken = Token("$", "$");
                }
                matched = true;
            } else {
                Table errorTable;
                errorTable.format().font_background_color(Color::red).font_color(Color::white).font_style({FontStyle::bold});
                string errmsg = "Syntax Error: Expected '" + topSymbol + "' but found '" + tokenLexeme + "'";
                errorTable.add_row({errmsg});
                cout<<traceTable<<endl;
                cout<<errorTable<<endl;
                error = true;
                return false;
            }
        } else {
            matched = false;
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
                Table errorTable;
                errorTable.format().font_background_color(Color::red).font_color(Color::white).font_style({FontStyle::bold});
                string errmsg =  "Syntax Error: Unexpected token '(" + tokenLexeme + "," + tokenType + ")'";
                errorTable.add_row({errmsg});
                cout<<traceTable<<endl;
                cout<<errorTable<<endl;
                error = true;
                return false;
            }
        }
        actionStr = regex_replace(actionStr, regex("^\\s+|\\s+$"), "");

        traceTable.add_row({stackStr, inputStr, actionStr});
        auto &new_row = traceTable[rowIndex++];
        new_row[2].format().width(60);
        if(matched) {
            new_row.format()
                .font_style({FontStyle::bold})
                .font_color(Color::green);
        } else if (error) {
            new_row.format()
                .font_style({FontStyle::bold})
                .font_color(Color::red);
        } else {
            new_row.format()
                .font_style({FontStyle::bold})
                .font_color(Color::yellow);
        }
    }

    if (tokenIndex < tokens.size()) {
        cerr << "\nSyntax Error: Extra input found after parsing\n";
        cout << tokenIndex << " " << tokens.size() << endl;
        cout << tokens[tokenIndex].getLexeme() << endl;
        return false;
    }

    cout << traceTable << endl;
    Table successTable;
    successTable.format().font_background_color(Color::green).font_color(Color::white).font_style({FontStyle::bold})
                .border_top(" ")
                .border_bottom(" ")
                .border_left(" ")
                .border_right(" ");
    successTable.add_row({"Parsing successful!"});
    cout<<successTable<<endl;
    return true;
}


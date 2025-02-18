#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "RegexParser.h"
#include "RegexToNFA.h"
#include "NFAtoDFA.h"
#include "DFAtoLexer.h"

using namespace std;

int main() {
    vector<pair<string, string>> tokenDefinitions;
    string input;
    
    // Read token definitions and input from stdin
    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        if (line[0] == '"') { // Input string starts here
            input = line.substr(1, line.size() - 2);
            break;
        }
        size_t pos = line.find(' ');
        if (pos != string::npos) {
            string tokenName = line.substr(0, pos);
            string regex = line.substr(pos + 1);
            tokenDefinitions.push_back({tokenName, regex});
        }
    }

    // Convert each regex to DFA
    vector<pair<string, DFA>> dfas;
    for (const auto& [tokenName, regex] : tokenDefinitions) {
        string postfix = InfixToPostfix(regex);
        NFA nfa = RegexToNFA(postfix);
        DFA dfa = NFAtoDFA(nfa);
        dfas.push_back({tokenName, dfa});
    }

    // Run the lexer on the input string
    DFAtoLexer lexer(dfas);
    vector<pair<string, string>> tokens = lexer.tokenize(input);

    // Output tokens
    for (const auto& [tokenName, lexeme] : tokens) {
        cout << tokenName << " , \"" << lexeme << "\"" << endl;
    }
    
    return 0;
}

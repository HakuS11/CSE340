#include "DFAtoLexer.h"
#include <iostream>

Lexer::Lexer(const DFA& dfa) : dfa(dfa) {}

std::vector<Token> Lexer::Tokenize(const std::string& input) {
    std::vector<Token> tokens;
    std::string lexeme;
    int start_index = 0;
    dfa.Reset();

    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        dfa.Move(c);
        
        if (dfa.GetStatus() == ACCEPT) {
            lexeme += c;
        } else if (dfa.GetStatus() == FAIL) {
            if (!lexeme.empty()) {
                tokens.push_back({"TOKEN", lexeme});
                lexeme.clear();
            }
            dfa.Reset();
            start_index = i;
        } else {
            lexeme += c;
        }
    }
    
    if (dfa.GetAccepted() && !lexeme.empty()) {
        tokens.push_back({"TOKEN", lexeme});
    }
    
    return tokens;
}

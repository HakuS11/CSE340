#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <map>

using namespace std;

// Define operator precedence
int Precedence(char op) {
    if (op == '*') return 3; // Kleene star
    if (op == '.') return 2; // Concatenation
    if (op == '|') return 1; // Alternation
    return 0;
}

// Check if a character is an operand (alphabet or digit)
bool IsOperand(char c) {
    return isalnum(c);
}

// Convert infix regex to postfix using Shunting Yard Algorithm
string InfixToPostfix(const string &infix) {
    stack<char> ops;
    string postfix = "";

    for (size_t i = 0; i < infix.size(); i++) {
        char c = infix[i];
        
        if (IsOperand(c)) {
            postfix += c;
        } else if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop();
        } else {
            while (!ops.empty() && ops.top() != '(' && Precedence(ops.top()) >= Precedence(c)) {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);
        }
    }
    while (!ops.empty()) {
        postfix += ops.top();
        ops.pop();
    }
    return postfix;
}

// Token structure
struct TokenDef {
    string name;
    string regex;
};

// Function to parse token definitions and return a vector of TokenDef
vector<TokenDef> ParseTokenDefinitions(const string &input) {
    vector<TokenDef> tokens;
    stringstream ss(input);
    string segment;

    while (getline(ss, segment, ',')) {
        size_t pos = segment.find(' ');
        if (pos != string::npos) {
            string name = segment.substr(0, pos);
            string regex = segment.substr(pos + 1);
            tokens.push_back({name, regex});
        }
    }
    return tokens;
}

int main() {
    string input;
    cout << "Enter token definitions (comma-separated, ending with #):\n";
    getline(cin, input);

    size_t hashPos = input.find('#');
    if (hashPos != string::npos) {
        input = input.substr(0, hashPos); // Remove everything after '#'
    }
    
    vector<TokenDef> tokens = ParseTokenDefinitions(input);
    
    cout << "Converted Token Definitions with Postfix Notation:\n";
    for (const auto &token : tokens) {
        cout << token.name << " -> " << InfixToPostfix(token.regex) << endl;
    }
    return 0;
}

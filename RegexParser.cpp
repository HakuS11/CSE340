#include "RegexParser.h"
#include <iostream>
#include <stack>

int Precedence(char op) {
    if (op == '*') return 3;
    if (op == '.') return 2;
    if (op == '|') return 1;
    return 0;
}

bool IsOperand(char c) {
    return isalnum(c);
}

std::string InfixToPostfix(const std::string& infix) {
    std::stack<char> ops;
    std::string postfix;
    for (char c : infix) {
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
            while (!ops.empty() && Precedence(ops.top()) >= Precedence(c)) {
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

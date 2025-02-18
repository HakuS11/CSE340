#include <iostream>
#include <stack>
#include <string>
#include <cctype>

// Function to determine operator precedence
int Precedence(char op) {
    if (op == '*') return 3; // Kleene star has the highest precedence
    if (op == '.') return 2; // Concatenation
    if (op == '|') return 1; // Union (OR)
    return 0; // Default for non-operators
}

// Function to check if a character is an operand (alphanumeric)
bool IsOperand(char c) {
    return isalnum(c);
}

// Function to insert explicit concatenation operators
std::string InsertConcatenation(const std::string& regex) {
    std::string result;
    for (size_t i = 0; i < regex.length(); ++i) {
        result += regex[i];
        if (i + 1 < regex.length()) {
            char current = regex[i];
            char next = regex[i + 1];
            // Insert '.' if:
            // 1. Current character is an operand and next character is an operand.
            // 2. Current character is an operand and next character is '('.
            // 3. Current character is ')' and next character is an operand.
            // 4. Current character is '*' and next character is an operand or '('.
            if ((IsOperand(current) && IsOperand(next)) ||
                (IsOperand(current) && next == '(' ||
                (current == ')' && IsOperand(next)) ||
                (current == '*' && (IsOperand(next) || next == '('))) {
                result += '.';
            }
        }
    }
    return result;
}

// Function to convert infix regex to postfix notation
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
            if (!ops.empty() && ops.top() == '(') {
                ops.pop(); // Pop '('
            } else {
                throw std::runtime_error("Mismatched parentheses");
            }
        } else {
            while (!ops.empty() && Precedence(ops.top()) >= Precedence(c)) {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);
        }
    }
    while (!ops.empty()) {
        if (ops.top() == '(' || ops.top() == ')') {
            throw std::runtime_error("Mismatched parentheses");
        }
        postfix += ops.top();
        ops.pop();
    }
    return postfix;
}

int main() {
    std::string regex;
    std::cout << "Enter a regular expression: ";
    std::cin >> regex;

    try {
        std::string with_concat = InsertConcatenation(regex);
        std::string postfix = InfixToPostfix(with_concat);
        std::cout << "Postfix notation: " << postfix << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

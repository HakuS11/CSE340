#include <iostream>
#include <string>
#include <unordered_set>
using namespace std;

//--------------------------------------------------------------
// enum TokenType
//--------------------------------------------------------------
enum TokenType
{
    ID,
    NUM,
    OP,
    KEYWORD,
    EOS,     // end of string
    INVALID
};
const string tokenNames[] = { "ID", "NUM", "OP", "KEYWORD", "EOS", "INVALID" };

//--------------------------------------------------------------
// class Token
//--------------------------------------------------------------
class Token
{
public:
    Token(TokenType t, string v) : type(t), value(v) {};
    Token() : type(EOS), value("") {};
    TokenType type;
    string value;
};

//--------------------------------------------------------------
// class Lexer
//--------------------------------------------------------------
class Lexer
{
public:
    Lexer(const string& input) : input(input), pos(0) {
        // Initialize keywords
        keywords = {"if", "else", "while", "return"};
    }
    Token getToken();
private:
    string input;
    size_t pos;
    unordered_set<string> keywords;

    // DFA for identifiers
    bool isIdentifier(const string& str);
    // DFA for numbers
    bool isNumber(const string& str);
    // DFA for keywords
    bool isKeyword(const string& str);
};

//--------------------------------------------------------------
// DFA for identifiers
//--------------------------------------------------------------
bool Lexer::isIdentifier(const string& str) {
    if (str.empty() || !isalpha(str[0])) return false;
    for (char c : str) {
        if (!isalnum(c)) return false;
    }
    return true;
}

//--------------------------------------------------------------
// DFA for numbers
//--------------------------------------------------------------
bool Lexer::isNumber(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

//--------------------------------------------------------------
// DFA for keywords
//--------------------------------------------------------------
bool Lexer::isKeyword(const string& str) {
    return keywords.find(str) != keywords.end();
}

//--------------------------------------------------------------
// return next token from the input string
//--------------------------------------------------------------
Token Lexer::getToken()
{
    //---- Skip whitespace
    while (pos < input.size() && isspace(input[pos]))
        pos++;

    //---- check for EOS
    if (pos == input.size())
        return Token(EOS, "");

    //---- get next character from input
    char nextChar = input[pos];

    // Check for identifiers or keywords
    if (isalpha(nextChar))
    {
        string value;
        while (pos < input.size() && isalnum(input[pos]))
            value += input[pos++];

        if (isKeyword(value))
            return Token(KEYWORD, value);
        else if (isIdentifier(value))
            return Token(ID, value);
        else
            return Token(INVALID, value);
    }
    // Check for numbers
    else if (isdigit(nextChar))
    {
        string value;
        while (pos < input.size() && isdigit(input[pos]))
            value += input[pos++];

        if (isNumber(value))
            return Token(NUM, value);
        else
            return Token(INVALID, value);
    }
    // Check for operators
    else if (nextChar == '+' || nextChar == '-' || nextChar == '*' || nextChar == '/' || nextChar == '=')
    {
        return Token(OP, string(1, input[pos++]));
    }
    // Invalid token
    else
    {
        return Token(INVALID, string(1, input[pos++]));
    }
}

//--------------------------------------------------------------
// main
//--------------------------------------------------------------
int main()
{
    string input;
    getline(cin, input);
    Lexer lexer(input);
    Token token;
    while ((token = lexer.getToken()).type != EOS)
        cout << "Type: " << tokenNames[token.type] << "\t Value: " << token.value << endl;
    return 0;
}

#include <iostream>
#include <string>
#include <cctype>

using namespace std;

string input; //Input string to be tested
int pos = 0; //Position of string

//All CFG instantiation
void S();
void E();
void E_prime();
void T();
void T_prime();
void F();

//Checking the char at position for valid char
void match(char expected) {
    if (input[pos] == expected) {
        pos++;
    } else {
        cout << "Does not recognize char\n";
        exit(1);
    }
}

//S -> E
void S() {
    E();
}

void E() {
    //S->T
    T();
    //S->E'
    E_prime();
}

void E_prime() {
    //E'-> +TE'
    if (input[pos] == '+') {
        match('+');
        T();
        E_prime();
    //E'-> -TE'
    } else if (input[pos] == '-') {
        match('-');
        T();
        E_prime();
    //E' -> ε by doing nothing
    } 
}

void T() {
    //T->F
    F();
    //T->T'
    T_prime();
}

void T_prime() {
    //T'->*FT'
    if (input[pos] == '*') {
        match('*');
        F();
        T_prime();
    //T'->/FT'
    } else if (input[pos] == '/') {
        match('/');
        F();
        T_prime();
    //T' -> ε by doing nothing
    }
}

void F() {
    //F->(E)
    if (input[pos] == '(') {
        match('(');
        E();
        match(')');
    //F-> num  
    //num is checked for with isdigit
    } else if (isdigit(input[pos])) {
        while (isdigit(input[pos])) {
            pos++;
        }
    } else {
        cout << "Does not recognize char\n";
        exit(1);
    }
}

int main() {
    //input = "(3+5)*7"; //Valid token 1
    //input = "7+2"; //Valid token 2
    input = "2++1"; //Invalid token 1
    pos = 0;

    S(); //Start parsing at S
    
    if (input[pos] == '\0') {
        cout << "Recognizes all char\n";
    } else {
        cout << "Does not recognize char\n";
    }

    return 0;
}

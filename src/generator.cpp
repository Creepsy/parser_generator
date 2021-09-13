#include <iostream>
#include <fstream>

#include "rule_lexer.h"

int main() {
    std::ifstream input{"../example_rules.txt"};

    if(!input.is_open()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return -1;
    }

    rule_lexer lex{input};

    while(!lex.end()) {
        token next = lex.next_unignored_token();
        std::cout << next.type << ", " << next.identifier << std::endl;
    }
   
    return 0;
}
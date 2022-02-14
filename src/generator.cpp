#include <iostream>
#include <fstream>

#include "type_parser/type_parser.h"

int main() {
    std::ifstream input{"../example_parser.types"};

    if(!input.is_open()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return -1;
    }

    type_lexer lex{input};

    type_parser::TypeParser parser{lex};

    while(!parser.end()) {
        std::cout << parser.parse_next_type() << "\n" << std::endl;
    }

    // while(!lex.end()) {
    //     token next = lex.next_unignored_token();
    //     std::cout << next.type << ", " << next.identifier << std::endl;
    // }
   
    return 0;
}
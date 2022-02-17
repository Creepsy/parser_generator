#include <iostream>
#include <fstream>

#include "type_parser/type_parser.h"
#include "rule_parser/rule_parser.h"

int main() {
    std::ifstream rule_input{"../example_parser.rules"};

    if(!rule_input.is_open()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return -1;
    }

    rule_lexer::rule_lexer rule_lex{rule_input};
    rule_parser::RuleParser rule_pars{rule_lex};

    while(!rule_pars.end()) {
        std::cout << rule_pars.parse_next_rule() << std::endl;
    }

    std::cout << std::endl;

    std::ifstream type_input{"../example_parser.types"};

    if(!type_input.is_open()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return -1;
    }

    type_lexer::type_lexer type_lex{type_input};
    type_parser::TypeParser type_pars{type_lex};

    while(!type_pars.end()) {
        std::cout << type_pars.parse_next_type() << "\n" << std::endl;
    }

    // while(!lex.end()) {
    //     token next = lex.next_unignored_token();
    //     std::cout << next.type << ", " << next.identifier << std::endl;
    // }
   
    return 0;
}
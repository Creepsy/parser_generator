#include <iostream>
#include <fstream>

#include "type_parser/type_parser.h"
#include "type_parser/type_validator.h"

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

    std::vector<type_parser::TypeDefinition> types;

    while(!type_pars.end()) {
        types.push_back(type_pars.parse_next_type());

        std::cout << types.back() << "\n" << std::endl;
    }

    type_parser::validate_types(types);
    type_parser::TypeInfoTable type_infos = type_parser::construct_type_info_table(types);

    std::cout << type_infos << std::endl;
   
    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "rule_lexer.h"
#include "rule_parser/parser_infos.h"
#include "rule_parser/rule_parser.h"

int main() {
    std::ifstream input{"../example_rules.txt"};

    if(!input.is_open()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return -1;
    }

    rule_lexer lex{input};
    rparser::rule_parser pars{lex};

    parser_infos::parser_info p_info = pars.parse_parser_infos();
    input.close();

    std::cout << "Types: " << std::endl;
    for(const parser_infos::type_definition& type : p_info.types) std::cout << type << std::endl;
    
    std::cout << std::endl;

    std::cout << "Rules: " << std::endl;
    for(const parser_infos::rule& r : p_info.parser_rules) std::cout << r << std::endl;

    std::cout << std::endl;

    std::cout << "Entry rules: " << std::endl;
    for(const parser_infos::rule& r : p_info.entry_rules) std::cout << r << std::endl;

    return 0;
}
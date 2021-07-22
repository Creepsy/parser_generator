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

    for(const parser_infos::type_definition& type : p_info.types) std::cout << type << std::endl;
    
    /*while(true) {
        token t = lex.next_unignored_token();
        std::cout << rparser::TOKEN_TYPE_TO_STR[t.type] << " -> " << t.identifier
                  << ", [" << t.pos.start_line << "," << t.pos.start_column << "] to [" << t.pos.end_line << "," << t.pos.end_column << "]" << std::endl;
        if(t.type == token::END_OF_FILE) break;
    }*/

    input.close();

    return 0;
}
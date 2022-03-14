#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

#include "type_parser/type_parser.h"
#include "type_parser/type_validator.h"

#include "rule_parser/rule_parser.h"
#include "rule_parser/rule_validator.h"

#include "generator/parser/states.h"
#include "generator/parser/parser_generator.h"
#
#include "generator/code/type_generation.h"
#include "generator/code/parser_generation.h"

int main() {
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
    }

    type_input.close();



    std::ifstream rule_input{"../example_parser.rules"};

    if(!rule_input.is_open()) {
        std::cerr << "Unable to open input file!" << std::endl;
        return -1;
    }

    rule_lexer::rule_lexer rule_lex{rule_input};
    rule_parser::RuleParser rule_pars{rule_lex};

    std::vector<rule_parser::RuleDefinition> rules;

    while(!rule_pars.end()) {
        rules.push_back(rule_pars.parse_next_rule());
    }

    rule_input.close();



    type_parser::validate_types(types);
    type_parser::TypeInfoTable type_infos = type_parser::construct_type_info_table(types);

    rule_parser::validate_rules(rules, type_infos);
    states::StartTokensTable start_table = states::construct_start_token_table(rules, type_infos);

    parser_generator::StatesInfo states_info = parser_generator::generate_parser_states(rules, type_infos, start_table);

    for(const std::pair<states::State, size_t>& state : states_info.parser_states) {
        std::cout << state.second << ". " << state.first;
    }

    std::cout << "\nStart-State: " << states_info.start_state << std::endl;

    code_generator::LexerFileInfo lexer_info{"", "lexer"};
    code_generator::ParserFileInfo parser_info{"types", "parser", "parser", "Parser"};

    code_generator::RuleIDMap rule_mappings = code_generator::generate_rule_ids(rules);

    //std::ofstream types_out{"../test/types.h"};
    std::ofstream header_out("../test/parser.h");
    std::ofstream source_out{"../test/parser.cpp"};

    //code_generator::generate_types_code(types_out, parser_info, lexer_info, types); 
    //types_out.close();

    code_generator::generate_parser_header_code(header_out, parser_info, lexer_info, states_info, rule_mappings);
    header_out.close();

    code_generator::generate_parser_source_code(source_out, parser_info, lexer_info, states_info, rule_mappings, type_infos);
    source_out.close();

    return 0;
}
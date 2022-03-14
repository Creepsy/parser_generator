#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <string>

#include "type_parser/type_parser.h"
#include "type_parser/type_validator.h"

#include "rule_parser/rule_parser.h"
#include "rule_parser/rule_validator.h"

#include "generator/parser/states.h"
#include "generator/parser/parser_generator.h"
#
#include "generator/code/type_generation.h"
#include "generator/code/parser_generation.h"

int main(int argc, char* argv[]) {
    if(argc != 9) {
        std::cerr << "Usage: " << argv[0] << " <lexer_dir> <lexer_name> <output_dir> <types_name> <namespace_name> <parser_name> <types_input> <rules_input>" << std::endl;
        return -1;
    }

    const code_generator::LexerFileInfo lexer_info{argv[1], argv[2]};
    const code_generator::ParserFileInfo parser_info{argv[3], argv[4], argv[5], argv[6]};

    const std::string types_input_path = argv[7];
    const std::string rules_input_path = argv[8];

    std::ifstream type_input{types_input_path};

    if(!type_input.is_open()) {
        std::cerr << "Unable to open types file!" << std::endl;
        return -1;
    }

    type_lexer::type_lexer type_lex{type_input};
    type_parser::TypeParser type_pars{type_lex};

    std::vector<type_parser::TypeDefinition> types;

    while(!type_pars.end()) {
        types.push_back(type_pars.parse_next_type());
    }

    type_input.close();



    std::ifstream rule_input{rules_input_path};

    if(!rule_input.is_open()) {
        std::cerr << "Unable to open rules file!" << std::endl;
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
    code_generator::RuleIDMap rule_mappings = code_generator::generate_rule_ids(rules);

    std::ofstream types_out{parser_info.output_dir + parser_info.types_name + ".h"};
    std::ofstream header_out(parser_info.output_dir + parser_info.parser_name + ".h");
    std::ofstream source_out{parser_info.output_dir + parser_info.parser_name + ".cpp"};

    code_generator::generate_types_code(types_out, parser_info, lexer_info, types); 
    types_out.close();

    std::cout << "Types header successfully created!" << std::endl;

    code_generator::generate_parser_header_code(header_out, parser_info, lexer_info, states_info, rule_mappings);
    header_out.close();

    std::cout << "Parser header successfully created!" << std::endl;

    code_generator::generate_parser_source_code(source_out, parser_info, lexer_info, states_info, rule_mappings, type_infos);
    source_out.close();

    std::cout << "Parser source successfully created!" << std::endl;

    std::cout << "Finished!" << std::endl;

    return 0;
}
#include "parser_generation.h"

#include <algorithm>
#include <vector>
#include <iterator>
#include <string>

using namespace code_generator;

//helper functions
void generate_element_type_enum(std::ostream& target, const parser_generator::ParserInfo& states_info, const size_t indentation_level = 1);
std::set<std::string> get_used_type_names(const parser_generator::ParserInfo& states_info);

void generate_element_type_enum(std::ostream& target, const parser_generator::ParserInfo& states_info, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    std::set<std::string> used_types = get_used_type_names(states_info);

    target << indentation << "enum ElementType {\n";

    for(auto type_it = used_types.begin(); type_it != used_types.end(); type_it++) {
        target << indentation << "\t" << *type_it;

        if(type_it != --used_types.end())
            target << ",";
        target << "\n";
    }

    target << indentation << "};\n\n";
}

std::set<std::string> get_used_type_names(const parser_generator::ParserInfo& states_info) {
    std::set<std::string> used_types;
    for(const std::pair<states::State, size_t>& state : states_info.parser_states) {
        for(const states::Action& action : state.first.actions) {
            std::transform(action.possible_lookaheads.begin(), action.possible_lookaheads.end(), std::inserter(used_types, used_types.end()),
                [](const rule_parser::Argument& to_convert) -> std::string { 
                    std::string type_str;

                    if(to_convert.is_token)
                        type_str += "T_";

                    if(to_convert.is_vector)
                        type_str += "VEC_";

                    std::transform(to_convert.identifier.begin(), to_convert.identifier.end(), std::back_inserter(type_str), toupper);

                    return type_str;
                }
            );
        }
    }

    return used_types;
}



void code_generator::generate_parser_header_code(std::ostream& target, const ParserFileInfo& parser_info, 
        const LexerFileInfo& lexer_info, const parser_generator::ParserInfo& states_info) {
    target << "#pragma once\n\n"
           << "#include <any>\n"
           << "#include <cstddef>\n"
           << "#include <stack>\n\n"
           << "#include \"" << lexer_info.path << lexer_info.name << ".h\"\n"
           << "#include \"" << parser_info.types_path << ".h\"\n\n"
           << "namespace " << parser_info.namespace_name << " {\n";

    generate_element_type_enum(target, states_info);

    target << "\tstruct ParseStackElement {\n"
           << "\t\tstd::any element;\n"
           << "\t\tElementType type;\n"
           << "\t\tsize_t state_id;\n"
           << "\t};\n\n";

    target << "\tclass " << parser_info.parser_name << " {\n"
           << "\t\tprivate:\n"
           << "\t\t\t" << lexer_info.name << "::" << lexer_info.name << " lex;\n"
           << "\t\t\t" << lexer_info.name << "::token curr;\n"
           << "\t\t\tstd::stack<ParseStackElement> parse_stack;\n\n"
           << "\t\t\tvoid next();\n" 
           << "\t\t\tsize_t goto_table(const ElementType parsed);\n"
           << "\t\tpublic:\n"
           << "\t\t\t" << parser_info.parser_name << "(" << lexer_info.name << "::" << lexer_info.name << " lex);\n"
           << "\t\t\tstd::any parse_next();\n"
           << "\t\t\tbool end();\n"
           << "\t\t\t~" << parser_info.parser_name << "();\n"
           << "\t};\n\n";

    target << "}\n";
}
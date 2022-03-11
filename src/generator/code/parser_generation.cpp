#include "parser_generation.h"

#include <algorithm>
#include <vector>
#include <iterator>
#include <string>

using namespace code_generator;

//helper functions
std::string type_to_enum_name(const rule_parser::Argument& to_convert);
std::set<std::string> get_used_type_enum_names(const parser_generator::StatesInfo& states_info);
void generate_element_type_enum(std::ostream& target, const parser_generator::StatesInfo& states_info, const size_t indentation_level = 1);
void generate_goto_table(std::ostream& target, const parser_generator::StatesInfo& states_info, const size_t indentation_level = 1);

std::string type_to_enum_name(const rule_parser::Argument& to_convert) {
    std::string type_str = (to_convert.is_vector) ? "VEC_" : "";

    std::transform(to_convert.identifier.begin(), to_convert.identifier.end(), std::back_inserter(type_str), toupper);

    if(to_convert.is_token)
        type_str = "TOKEN";

    return type_str;
}

std::set<std::string> get_used_type_enum_names(const parser_generator::StatesInfo& states_info) {
    std::set<std::string> used_types;
    for(const std::pair<states::State, size_t>& state : states_info.parser_states) {
        for(const states::Action& action : state.first.actions) {
            std::transform(action.possible_lookaheads.begin(), action.possible_lookaheads.end(), std::inserter(used_types, used_types.end()), type_to_enum_name);
        }
    }

    return used_types;
}

void generate_element_type_enum(std::ostream& target, const parser_generator::StatesInfo& states_info, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    std::set<std::string> used_types = get_used_type_enum_names(states_info);

    target << indentation << "enum ElementType {\n";

    for(auto type_it = used_types.begin(); type_it != used_types.end(); type_it++) {
        target << indentation << "\t" << *type_it;

        if(type_it != --used_types.end())
            target << ",";
        target << "\n";
    }

    target << indentation << "};\n\n";
}

void generate_goto_table(std::ostream& target, const parser_generator::StatesInfo& states_info, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    target << indentation << "switch(this->get_state()) {\n";

    for(const std::pair<states::State, size_t>& state : states_info.parser_states) {
        target << indentation << "\tcase " << state.second << ":\n"
               << indentation << "\t\tswitch(parsed.type) {\n";

        for(const states::Action& action : state.first.actions) {
            if(action.type == states::Action::GOTO) {
                for(const rule_parser::Argument& possible_type : action.possible_lookaheads)
                    target << indentation << "\t\t\tcase ElementType::" << type_to_enum_name(possible_type) << ":\n";
                target << indentation << "\t\t\t\tparsed.state_id = " << std::get<size_t>(action.operand) << ";\n"
                       << indentation << "\t\t\t\tthis->parse_stack.push(std::move(parsed));\n"
                       << indentation << "\t\t\t\tbreak;\n";
            }
        }

        target << indentation << "\t\t\tdefault:\n"
               << indentation << "\t\t\t\tthis->throw_parse_error();\n"
               << indentation << "\t\tbreak;\n";
    }

    target << indentation << "\tdefault:\n"
           << indentation << "\t\tthis->throw_parse_error();\n"
           << indentation << "}\n";
}



RuleIDMap code_generator::generate_rule_ids(const std::vector<rule_parser::RuleDefinition>& rules) {
    RuleIDMap rule_ids;

    size_t id = 0;

    std::transform(rules.begin(), rules.end(), std::inserter(rule_ids, rule_ids.end()), 
        [&](const rule_parser::RuleDefinition& rule) -> std::pair<rule_parser::RuleDefinition, size_t> {
            return std::make_pair(rule, id++);
        }
    );

    return rule_ids;
}

void code_generator::generate_parser_header_code(std::ostream& target, const ParserFileInfo& parser_info, 
        const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const size_t rule_count) {
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
           << "\t\t\tvoid shift(const size_t state_id);\n" 
           << "\t\t\tvoid goto(ParseStackElement&& parsed);\n"
           << "\t\t\tvoid throw_parse_error();\n"
           << "\t\t\tsize_t get_state();\n\n";

    for(size_t rule_id = 0; rule_id < rule_count; rule_id++) {
        target << "\t\t\tvoid reduce_" << rule_id << "();\n";
    }

    target << "\t\tpublic:\n"
           << "\t\t\t" << parser_info.parser_name << "(" << lexer_info.name << "::" << lexer_info.name << " lex);\n"
           << "\t\t\tParseStackElement parse_next();\n"
           << "\t\t\tbool end();\n"
           << "\t\t\t~" << parser_info.parser_name << "();\n"
           << "\t};\n\n";

    target << "}\n";
}

void code_generator::generate_parser_source_code(std::ostream& target, const ParserFileInfo& parser_info,
            const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const RuleIDMap& rule_mappings) {
    target << "#include \"" << parser_info.parser_name << ".h\"\n\n"
           << "#include <utility>\n"
           << "#include <stdexcept>\n\n"
           << "using namespace " << parser_info.namespace_name << ";\n\n";

    target << parser_info.parser_name << "::" << parser_info.parser_name << "(" << lexer_info.name << "::" << lexer_info.name << " lex) : lex(lex) {\n"
           << "\tthis->curr = this->lex.next_unignored_token();\n"
           << "}\n\n";

    //TODO
    target << "ParseStackElement " << parser_info.parser_name << "::parse_next() {\n"
           << "}\n\n";
    
    target << "bool " << parser_info.parser_name << "::end() {\n"
           << "\treturn this->curr.type == " << lexer_info.name << "::token::END_OF_FILE;\n"
           << "}\n\n";

    target << parser_info.parser_name << "::~" << parser_info.parser_name << "() {\n"
           << "}\n\n\n\n";

    target << "void " << parser_info.parser_name << "::shift(const size_t state_id) {\n"
           << "\tthis->parse_stack.push(ParseStackElement{this->curr, ElementType::TOKEN, state_id});\n"
           << "\tthis->curr = this->lex.next_unignored_token();\n"
           << "}\n\n";

    target << "void " << parser_info.parser_name << "::goto(ParseStackElement&& parsed) {\n";
    generate_goto_table(target, states_info);
    target << "}\n\n";

    target << "void " << parser_info.parser_name << "::throw_parse_error() {\n"
           << "\tconstexpr auto format_position = [](size_t line, size_t column) -> std::string { return \"[L:\" + std::to_string(line) + \", C:\" + std::to_string(column) + \"]\"; };\n"
           << "\tthrow std::runtime_error(\"Error while parsing \" + this->curr.identifier + \" at \" + format_position(this->curr.pos) +  \"! \");\n"
           << "}\n\n";

    target << "size_t " << parser_info.parser_name << "::get_state() {\n"
           << "\treturn (this->parse_stack.empty()) ? " << states_info.start_state << " : this->parse_stack.top().state_id;\n"
           << "}\n\n";

    //TODO reduce rules
}
#include "parser_generation.h"

#include <algorithm>
#include <vector>
#include <iterator>
#include <string>
#include <iostream>

using namespace code_generator;

//helper functions
std::string type_to_enum_name(const rule_parser::Argument& to_convert);
std::string type_to_cpp_type(const rule_parser::Argument& to_convert, const LexerFileInfo& lexer_info);
std::set<std::string> get_used_type_enum_names(const std::vector<rule_parser::RuleDefinition>& rules);
void generate_reduce_declaration(std::ostream& target, const std::pair<rule_parser::RuleDefinition, size_t>& rule_mapping, const size_t indentation_level = 1);
void generate_element_type_enum(std::ostream& target, const std::vector<rule_parser::RuleDefinition>& rules, const size_t indentation_level = 1);
void generate_goto_table(std::ostream& target, const parser_generator::StatesInfo& states_info, const size_t indentation_level = 1);
void generate_parser_states(std::ostream& target, const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const RuleIDMap& rule_mappings, const size_t indentation_level = 1);
void convert_arg(std::ostream& target, const std::string& source_identifier, const std::string& target_identifier, const rule_parser::Argument& source_type, const rule_parser::Argument& target_type, const LexerFileInfo& lexer_info, const size_t indentation_level = 1);
void generate_rule_reduction(std::ostream& target, const std::pair<rule_parser::RuleDefinition, size_t>& rule_mapping, const ParserFileInfo& parser_info, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos);
void generate_rule_result(std::ostream& target, const rule_parser::RuleDefinition& rule, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos, const size_t indentation_level = 1);
void generate_create_new_result(std::ostream& target, const rule_parser::RuleDefinition& rule, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos, const size_t indentation_level = 1);
void generate_create_vector_result(std::ostream& target, const rule_parser::RuleDefinition& rule, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos, const size_t indentation_level = 1);


std::string type_to_enum_name(const rule_parser::Argument& to_convert) {
    std::string type_str = (to_convert.is_vector) ? "VEC_" : "";

    std::transform(to_convert.identifier.begin(), to_convert.identifier.end(), std::back_inserter(type_str), toupper);

    if(to_convert.is_token)
        type_str = "TOKEN";

    return type_str;
}

std::string type_to_cpp_type(const rule_parser::Argument& to_convert, const LexerFileInfo& lexer_info) {
    if(to_convert.is_token)
        return lexer_info.name + "::token";

    if(to_convert.is_vector)
        return "std::vector<std::shared_ptr<" + to_convert.identifier + ">>";

    return "std::shared_ptr<" + to_convert.identifier + ">";
}

std::set<std::string> get_used_type_enum_names(const std::vector<rule_parser::RuleDefinition>& rules) {
    std::set<std::string> used_types;

    for(const rule_parser::RuleDefinition& rule : rules) {
        used_types.insert(type_to_enum_name(rule.result.type));
        std::transform(rule.arguments.begin(), rule.arguments.end(), std::inserter(used_types, used_types.end()), type_to_enum_name);
    }

    return used_types;
}

void generate_reduce_declaration(std::ostream& target, const std::pair<rule_parser::RuleDefinition, size_t>& rule_mapping, const size_t indentation_level) {
    target << "\t\t\t";
    target << ((rule_mapping.first.is_entry) ? "ParseStackElement" : "void");
    target << " reduce_" << rule_mapping.second << "();\n";
}

void generate_element_type_enum(std::ostream& target, const std::vector<rule_parser::RuleDefinition>& rules, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    std::set<std::string> used_types = get_used_type_enum_names(rules);

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
               << indentation << "\t\t}\n"
               << indentation << "\t\tbreak;\n";
    }

    target << indentation << "\tdefault:\n"
           << indentation << "\t\tthis->throw_parse_error();\n"
           << indentation << "}\n";
}

void generate_parser_states(std::ostream& target, const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const RuleIDMap& rule_mappings, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    target << indentation << "while(true) {\n"
           << indentation << "\tswitch(this->get_state()) {\n";

    for(const std::pair<states::State, size_t>& state : states_info.parser_states) {
        target << indentation << "\t\tcase " << state.second << ":\n"
               << indentation << "\t\t\tswitch(this->curr.type) {\n";

        bool accept_on_default = false;

        for(const states::Action& action : state.first.actions) {
            if(action.type != states::Action::GOTO) {
                for(const rule_parser::Argument& lookahead : action.possible_lookaheads) {
                    target << indentation << "\t\t\t\tcase " << lexer_info.name << "::token::" << lookahead.identifier << ":\n";
                }
            }

            switch(action.type) {
                case states::Action::ACCEPT:
                    accept_on_default = true;
                    target << indentation << "\t\t\t\tdefault:\n"
                           << indentation << "\t\t\t\t\treturn this->reduce_" << rule_mappings.at(std::get<rule_parser::RuleDefinition>(action.operand)) << "();\n";
                    break;
                case states::Action::REDUCE:
                    target << indentation << "\t\t\t\t\tthis->reduce_" << rule_mappings.at(std::get<rule_parser::RuleDefinition>(action.operand)) << "();\n";
                    break;
                case states::Action::SHIFT:
                    target << indentation << "\t\t\t\t\tthis->shift(" << std::get<size_t>(action.operand) << ");\n";
                    break;
                default:
                    break;
            }

            target << indentation << "\t\t\t\t\tbreak;\n";

        }

        if(!accept_on_default)
            target << indentation << "\t\t\t\tdefault:\n"
                << indentation << "\t\t\t\t\tthis->throw_parse_error();\n"
                << indentation << "\t\t\t\t\tbreak;\n";

        target << indentation << "\t\t\t}\n"
               << indentation << "\t\t\tbreak;\n";

    }
    target << indentation << "\t\tdefault:\n"
           << indentation << "\t\t\tthis->throw_parse_error();\n"
           << indentation << "\tbreak;\n"
           << indentation << "\t}\n"
           << indentation << "}\n";
}

void convert_arg(std::ostream& target, const std::string& source_identifier, const std::string& target_identifier, const rule_parser::Argument& source_type, const rule_parser::Argument& target_type, const LexerFileInfo& lexer_info, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');
   
    std::string target_type_str = type_to_cpp_type(target_type, lexer_info);

    // const std::string arg_str = "arg_" + std::to_string(arg);
    // const std::string conv_str = "conv_" + std::to_string(arg);

    if(source_type.is_vector) {
        target << indentation << target_type_str << " " << target_identifier << ";\n";
        target << indentation << "std::transform(" << source_identifier << ".begin(), " << source_identifier << ".end(), std::back_inserter(" << target_identifier << "), \n"
                << indentation << "\t[](const std::shared_ptr<" << source_type.identifier << ">& ptr) -> std::shared_ptr<" << target_type.identifier << "> {\n"
                << indentation << "\t\treturn std::dynamic_pointer_cast<" << target_type.identifier << ">(ptr);\n"
                << indentation << "\t}\n"
                << indentation << ");\n";
    } else {
        target << indentation << target_type_str << " " << target_identifier << " = std::dynamic_pointer_cast<" << target_type.identifier << ">(" << source_identifier << ");\n";
    }
}

void generate_rule_reduction(std::ostream& target, const std::pair<rule_parser::RuleDefinition, size_t>& rule_mapping, const ParserFileInfo& parser_info, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos) {
    target << ((rule_mapping.first.is_entry) ? "ParseStackElement " : "void ") << parser_info.parser_name << "::reduce_" << rule_mapping.second << "() {\n";

    for(ssize_t arg = rule_mapping.first.arguments.size() - 1; arg >= 0; arg--) {
        const rule_parser::Argument target_type{rule_mapping.first.arguments[arg].is_token, rule_mapping.first.arguments[arg].is_vector, "TypeBase"};
        std::string type_str = type_to_cpp_type(target_type, lexer_info);
        target << "\t" << type_str << " arg_" << arg << " = std::any_cast<" << type_str << ">(this->parse_stack.top().element); this->parse_stack.pop();\n";
    }

    target << "\n";

    generate_rule_result(target, rule_mapping.first, lexer_info, type_infos);

    target << "}\n\n";
}

void generate_rule_result(std::ostream& target, const rule_parser::RuleDefinition& rule, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    switch(rule.result.result_type) {
        case rule_parser::RuleResult::CREATE_NEW:
            generate_create_new_result(target, rule, lexer_info, type_infos, indentation_level);
            break;
        case rule_parser::RuleResult::PASS_ARG:
            {
                size_t arg_id = rule.result.argument_ids.front().value();
            
                if(rule.is_entry) {
                    std::string arg_str = "arg_";

                    if(!rule.result.type.is_token) {
                        rule_parser::Argument source_type{false, rule.result.type.is_vector, "TypeBase"};
                        convert_arg(target, "arg_" + std::to_string(arg_id), "conv_" + std::to_string(arg_id), source_type, rule.result.type, lexer_info, indentation_level);
                        arg_str = "conv_";
                    }
                
                    target << indentation << "return ParseStackElement{" << arg_str << arg_id << ", ElementType::" << type_to_enum_name(rule.result.type) << ", 0};\n";
                } else {
                    target << indentation << "this->goto_table(std::move(ParseStackElement{arg_" << arg_id << ", ElementType::" << type_to_enum_name(rule.result.type) << ", 0}));\n";
                }
            }
            break;
        case rule_parser::RuleResult::CREATE_VECTOR:
            generate_create_vector_result(target, rule, lexer_info, type_infos, indentation_level);
            break;
    }
}

void generate_create_new_result(std::ostream& target, const rule_parser::RuleDefinition& rule, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    std::string type_str = rule.result.type.identifier;
    std::string type_enum = type_to_enum_name(rule.result.type);

    for(size_t arg = 0; arg < rule.result.argument_ids.size(); arg++) {
        if(rule.result.argument_ids[arg].has_value()) {
            size_t arg_id = rule.result.argument_ids[arg].value();
            if(!rule.arguments[arg_id].is_token) {
                const rule_parser::Argument source_type{false, rule.arguments[arg_id].is_vector, "TypeBase"};
                const std::string target_type = type_infos.at(rule.result.type.identifier).parameters[arg].type;
                convert_arg(target, "arg_" + std::to_string(arg_id), "conv_" + std::to_string(arg_id), source_type, rule_parser::Argument{false, rule.arguments[arg_id].is_vector, target_type}, lexer_info, indentation_level);
            }
        }
    }

    target << indentation << "ParseStackElement result{";

    if(!rule.is_entry)
        target << "std::dynamic_pointer_cast<TypeBase>";

    target << "(std::make_shared<" << type_str << ">(";

    for(size_t arg = 0; arg < rule.result.argument_ids.size(); arg++) {
        if(arg != 0) target << ", ";

        if(rule.result.argument_ids[arg].has_value()) {
            size_t arg_id = rule.result.argument_ids[arg].value();
            const std::string arg_str = (!rule.arguments[arg_id].is_token) ? "conv_" : "arg_";
            target << "std::move(" << arg_str << rule.result.argument_ids[arg].value() << ")";
        } else {
            target << "{}";
        }
    }

    target << ")), ElementType::" << type_enum << ", 0};\n";

    if(rule.is_entry) {
        target << indentation << "return result;\n";
    } else {
        target << indentation << "this->goto_table(std::move(result));\n";
    }
}

void generate_create_vector_result(std::ostream& target, const rule_parser::RuleDefinition& rule, const LexerFileInfo& lexer_info, const type_parser::TypeInfoTable& type_infos, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    const rule_parser::Argument vector_type{rule.result.type.is_token, rule.result.type.is_vector, "TypeBase"};

    target << indentation << type_to_cpp_type(vector_type, lexer_info) << " collected;\n\n";

    for(size_t arg = 0; arg < rule.result.argument_ids.size(); arg++) {

        target << indentation;

        if(rule.result.argument_ids[arg].has_value()) {
            size_t arg_id = rule.result.argument_ids[arg].value();
            const rule_parser::Argument arg_type = rule.arguments[arg_id];

            if(arg_type.is_vector) {
                target << "std::copy(arg_" << arg_id << ".begin(), arg_" << arg_id << ".end(), std::back_inserter(collected));\n";
            } else {
                target << "collected.push_back(arg_" << arg_id << ");\n";
            }
        } else {
            target << "collected.push_back({});\n";
        }
    }
    target << "\n";

    std::string type_enum = type_to_enum_name(rule.result.type);

    if(rule.is_entry) {
        convert_arg(target, "collected", "conv_collected", vector_type, rule.result.type, lexer_info, indentation_level);
        target << indentation << "return ParseStackElement{std::move(conv_collected), ElementType::" << type_enum << ", 0};\n";
    } else {
        target << indentation << "this->goto_table(std::move(ParseStackElement{std::move(collected), ElementType::" << type_enum << ", 0}));\n";
    }   
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
        const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const RuleIDMap& rule_mappings) {
    target << "#pragma once\n\n"
           << "#include <any>\n"
           << "#include <cstddef>\n"
           << "#include <stack>\n\n"
           << "#include \"" << lexer_info.path << lexer_info.name << ".h\"\n"
           << "#include \"" << parser_info.types_path << ".h\"\n\n"
           << "namespace " << parser_info.namespace_name << " {\n";
    
    std::vector<rule_parser::RuleDefinition> rules;
    std::transform(rule_mappings.begin(), rule_mappings.end(), std::back_inserter(rules), 
        [](const std::pair<rule_parser::RuleDefinition, size_t>& entry) -> rule_parser::RuleDefinition {
            return entry.first;
        }
    );

    generate_element_type_enum(target, rules);

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
           << "\t\t\tvoid goto_table(ParseStackElement&& parsed);\n"
           << "\t\t\tvoid throw_parse_error();\n"
           << "\t\t\tsize_t get_state();\n\n";

    for(const std::pair<rule_parser::RuleDefinition, size_t>& rule_mapping : rule_mappings) {
        generate_reduce_declaration(target, rule_mapping);
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
            const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const RuleIDMap& rule_mappings, const type_parser::TypeInfoTable& type_infos) {
    target << "#include \"" << parser_info.parser_path << ".h\"\n\n"
           << "#include <utility>\n"
           << "#include <algorithm>\n"
           << "#include <vector>\n"
           << "#include <iterator>\n"
           << "#include <stdexcept>\n\n"
           << "using namespace " << parser_info.namespace_name << ";\n\n";

    target << parser_info.parser_name << "::" << parser_info.parser_name << "(" << lexer_info.name << "::" << lexer_info.name << " lex) : lex(lex) {\n"
           << "\tthis->curr = this->lex.next_unignored_token();\n"
           << "}\n\n";

    target << "ParseStackElement " << parser_info.parser_name << "::parse_next() {\n";
    generate_parser_states(target, lexer_info, states_info, rule_mappings);
    target << "}\n\n";
    
    target << "bool " << parser_info.parser_name << "::end() {\n"
           << "\treturn this->curr.type == " << lexer_info.name << "::token::END_OF_FILE;\n"
           << "}\n\n";

    target << parser_info.parser_name << "::~" << parser_info.parser_name << "() {\n"
           << "}\n\n\n\n";

    target << "void " << parser_info.parser_name << "::shift(const size_t state_id) {\n"
           << "\tthis->parse_stack.push(ParseStackElement{this->curr, ElementType::TOKEN, state_id});\n"
           << "\tthis->curr = this->lex.next_unignored_token();\n"
           << "}\n\n";

    target << "void " << parser_info.parser_name << "::goto_table(ParseStackElement&& parsed) {\n";
    generate_goto_table(target, states_info);
    target << "}\n\n";

    target << "void " << parser_info.parser_name << "::throw_parse_error() {\n"
           << "\tconstexpr auto format_position = [](size_t line, size_t column) -> std::string { return \"[L:\" + std::to_string(line) + \", C:\" + std::to_string(column) + \"]\"; };\n"
           << "\tthrow std::runtime_error(\"Error while parsing \" + this->curr.identifier + \" at \" + format_position(this->curr.pos.start_line, this->curr.pos.start_column) +  \"! \");\n"
           << "}\n\n";

    target << "size_t " << parser_info.parser_name << "::get_state() {\n"
           << "\treturn (this->parse_stack.empty()) ? " << states_info.start_state << " : this->parse_stack.top().state_id;\n"
           << "}\n\n";

    for(const std::pair<rule_parser::RuleDefinition, size_t>& rule_mapping : rule_mappings) 
        generate_rule_reduction(target, rule_mapping, parser_info, lexer_info, type_infos);
}
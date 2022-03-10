#include "type_generation.h"

#include <set>

using namespace code_generator;

//helper functions
std::string parameter_to_code(const type_parser::Parameter& to_convert, const LexerFileInfo& lexer_info);

std::string parameter_to_code(const type_parser::Parameter& to_convert, const LexerFileInfo& lexer_info) {
    std::string type_code;

    if(to_convert.is_vector)
        type_code += "std::vector<";

    if(to_convert.type == "Token") {
        type_code += lexer_info.name + "::token";
    } else {
        type_code += "std::shared_ptr<" + to_convert.type + ">";
    }

    if(to_convert.is_vector)
        type_code += ">";

    type_code += " " + to_convert.identifier + ";";

    return type_code;
}



void code_generator::generate_types_code(std::ostream& target, const ParserFileInfo& parser_info, const LexerFileInfo& lexer_info, const std::vector<type_parser::TypeDefinition>& types) {
    target << "#pragma once\n\n"
           << "#include <vector>\n"
           << "#include <memory>\n\n"
           << "#include \"" << lexer_info.path << lexer_info.name << ".h\"\n\n"
           << "namespace " << parser_info.namespace_name << " {\n";

    std::set<std::string> generated_types;

    //forward-declaration
    for(const type_parser::TypeDefinition& type : types) {
        if(!generated_types.contains(type.identifier)) {
            generated_types.insert(type.identifier);

            target << "\tstruct " << type.identifier << ";\n";

            for(const std::string& base_type : type.base_types) {
                if(!generated_types.contains(base_type)) {
                    generated_types.insert(base_type);

                    target << "\tstruct " << base_type << ";\n";
                }
            }
        }
    }

    generated_types.clear();
    target << "\n";

    for(const type_parser::TypeDefinition& type : types) {
        if(!generated_types.contains(type.identifier)) {
            generated_types.insert(type.identifier);

            generate_type_code(target, type, lexer_info);

            for(const std::string& base_type : type.base_types) {
                if(!generated_types.contains(base_type)) {
                    generated_types.insert(base_type);

                    generate_base_type_code(target, base_type);
                }
            }
        }
    }

    target << "}\n";
}

void code_generator::generate_type_code(std::ostream& target, const type_parser::TypeDefinition& type, const LexerFileInfo& lexer_info, const size_t intendation_level) {
    const std::string intendation = std::string(intendation_level, '\t');

    target << intendation << "struct " << type.identifier;

    if(!type.base_types.empty())
        target << " : ";

    for(auto base_type_it = type.base_types.begin(); base_type_it != type.base_types.end(); base_type_it++) {
        if(base_type_it != type.base_types.begin())
            target << ", ";

        target << "public " << *base_type_it;
    }

    target << " {\n";

    for(const type_parser::Parameter& par : type.parameters) {
        target << intendation << "\t" << parameter_to_code(par, lexer_info) << "\n";
    }

    target << intendation << "};\n\n";
}

void code_generator::generate_base_type_code(std::ostream& target, const std::string& base_type, const size_t intendation_level) {
    const std::string intendation = std::string(intendation_level, '\t');

    target << intendation << "struct " << base_type << " {\n"
           << intendation << "\tvirtual ~" << base_type << "(){}\n"
           << intendation << "};\n\n";
}
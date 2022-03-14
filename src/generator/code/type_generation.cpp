#include "type_generation.h"

#include <set>

using namespace code_generator;

//helper functions
std::string parameter_to_code(const type_parser::Parameter& to_convert, const LexerFileInfo& lexer_info, const bool is_reference = false);

std::string parameter_to_code(const type_parser::Parameter& to_convert, const LexerFileInfo& lexer_info, const bool is_reference) {
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

    if(is_reference)
        type_code += "&";

    type_code += " " + to_convert.identifier + "";

    return type_code;
}



void code_generator::generate_types_code(std::ostream& target, const ParserFileInfo& parser_info, const LexerFileInfo& lexer_info, const std::vector<type_parser::TypeDefinition>& types) {
    target << "#pragma once\n\n"
           << "#include <vector>\n"
           << "#include <memory>\n\n"
           << "#include \"" << lexer_info.path << lexer_info.name << ".h\"\n\n"
           << "namespace " << parser_info.namespace_name << " {\n";

    std::set<std::string> generated_types;

    generate_base_type_code(target, "TypeBase");

    //base types
    for(const type_parser::TypeDefinition& type : types) {
        if(!generated_types.contains(type.identifier)) {
            for(const std::string& base_type : type.base_types) {
                if(!generated_types.contains(base_type)) {
                    generated_types.insert(base_type);
                    generate_base_type_code(target, base_type);
                }
            }
        }
    }

    //types
    for(const type_parser::TypeDefinition& type : types) {
        type_parser::TypeDefinition modified = type;
        modified.base_types.insert("TypeBase");

        if(!generated_types.contains(modified.identifier)) {
            generated_types.insert(modified.identifier);
            generate_type_code(target, modified, lexer_info);
        }
    }

    target << "}\n";
}

void code_generator::generate_type_code(std::ostream& target, const type_parser::TypeDefinition& type, const LexerFileInfo& lexer_info, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    target << indentation << "struct " << type.identifier;

    if(!type.base_types.empty())
        target << " : ";

    for(auto base_type_it = type.base_types.begin(); base_type_it != type.base_types.end(); base_type_it++) {
        if(base_type_it != type.base_types.begin())
            target << ", ";

        target << "public " << *base_type_it;
    }

    target << " {\n";

    for(const type_parser::Parameter& par : type.parameters) {
        target << indentation << "\t" << parameter_to_code(par, lexer_info) << ";\n";
    }

    target << "\n"
           << indentation << "\t" << type.identifier << "(";


    for(size_t par = 0; par < type.parameters.size(); par++) {
        if(par != 0) target << ", ";
        target << "const " << parameter_to_code(type.parameters[par], lexer_info, true);
    }

    target << ") : ";

    for(size_t par = 0; par < type.parameters.size(); par++) {
        if(par != 0) target << ", ";
        target << type.parameters[par].identifier << "(" << type.parameters[par].identifier << ")";
    }

    target << "{}\n";

    target << indentation << "};\n\n";
}

void code_generator::generate_base_type_code(std::ostream& target, const std::string& base_type, const size_t indentation_level) {
    const std::string indentation = std::string(indentation_level, '\t');

    target << indentation << "struct " << base_type << " {\n"
           << indentation << "\tvirtual ~" << base_type << "(){}\n"
           << indentation << "};\n\n";
}
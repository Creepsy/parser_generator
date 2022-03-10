#include "parser_generation.h"

using namespace code_generator;

void code_generator::generate_parser_header_code(std::ostream& target, const ParserFileInfo& parser_info, 
        const LexerFileInfo& lexer_info, const parser_generator::ParserInfo& states_info) {
    target << "#pragma once\n\n"
           << "#include <any>\n\n"
           << "#include \"" << lexer_info.path << lexer_info.name << ".h\"\n"
           << "#include \"" << parser_info.types_path << ".h\"\n\n"
           << "namespace " << parser_info.namespace_name << " {\n";

    target << "\tclass " << parser_info.parser_name << " {\n"
           << "\t\tprivate:\n"
           << "\t\t\t" << lexer_info.name << "::" << lexer_info.name << " lex;\n"
           << "\t\tpublic:\n"
           << "\t\t\t" << parser_info.parser_name << "(" << lexer_info.name << "::" << lexer_info.name << " lex);\n"
           << "\t\t\tstd::any parse_next();\n"
           << "\t\t\tbool end();\n"
           << "\t\t\t~" << parser_info.parser_name << "();\n"
           << "\t}\n";

    target << "}\n";
}
#pragma once

#include <ostream>
#include <string>
#include <cstddef>
#include <vector>

#include "../../type_parser/types.h"

#include "generation_utils.h"

namespace code_generator {
    void generate_types_code(std::ostream& target, const ParserFileInfo& parser_info, const LexerFileInfo& lexer_info, const std::vector<type_parser::TypeDefinition>& types);
    void generate_type_code(std::ostream& target, const type_parser::TypeDefinition& type, const LexerFileInfo& lexer_info, const size_t indentation_level = 1);
    void generate_base_type_code(std::ostream& target, const std::string& base_type, const size_t indentation_level = 1);
}
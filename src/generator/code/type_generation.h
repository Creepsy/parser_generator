#pragma once

#include <ostream>
#include <string>
#include <cstddef>
#include <vector>

#include "../../type_parser/types.h"

namespace generator {
    struct LexerInfo {
        std::string path;
        std::string name;
    };

    void generate_types_code(std::ostream& target, const std::string& namespace_name, const LexerInfo& lexer_info, const std::vector<type_parser::TypeDefinition>& types);
    void generate_type_code(std::ostream& target, const type_parser::TypeDefinition& type, const LexerInfo& lexer_info, const size_t intendation_level = 1);
    void generate_base_type_code(std::ostream& target, const std::string& base_type, const size_t intendation_level = 1);
}
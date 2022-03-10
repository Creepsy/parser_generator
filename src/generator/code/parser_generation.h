#pragma once

#include <ostream>

#include "../parser/parser_generator.h"

#include "generation_utils.h"

namespace code_generator {
    void generate_parser_header_code(std::ostream& target, const ParserFileInfo& parser_info,
            const LexerFileInfo& lexer_info, const parser_generator::ParserInfo& states_info);
}
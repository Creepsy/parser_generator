#pragma once

#include <vector>

#include <type_lexer.h>

#include "../util/parser_base.h"

#include "types.h"

namespace type_parser {
    class TypeParser : public ParserBase<type_lexer::type_lexer, type_lexer::token> {
        public:
            TypeParser(type_lexer::type_lexer& lexer);
            TypeDefinition parse_next_type();
            ~TypeParser();
    };
}
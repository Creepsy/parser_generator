#pragma once

#include <vector>

#include <type_lexer.h>

#include "types.h"

namespace type_parser {
    class TypeParser {
        private:
            type_lexer lexer;
            token curr;

            void expect(const token::token_type to_expect);
            bool accept(const token::token_type to_check);
            token consume();
            token consume(const token::token_type to_expect);

            void throw_parse_err();
        public:
            TypeParser(type_lexer& lexer);
            TypeDefinition parse_next_type();
            bool end();
            ~TypeParser();
    };
}
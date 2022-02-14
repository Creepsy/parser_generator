#pragma once

#include <vector>

#include <type_lexer.h>

#include "types.h"

namespace type_parser {
    class TypeParser {
        private:
            type_lexer::type_lexer lexer;
            type_lexer::token curr;

            void expect(const type_lexer::token::token_type to_expect);
            bool accept(const type_lexer::token::token_type to_check);
            type_lexer::token consume();
            type_lexer::token consume(const type_lexer::token::token_type to_expect);

            void throw_parse_err();
        public:
            TypeParser(type_lexer::type_lexer& lexer);
            TypeDefinition parse_next_type();
            bool end();
            ~TypeParser();
    };
}
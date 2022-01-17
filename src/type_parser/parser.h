#pragma once

#include <type_lexer.h>
#include "types.h"

namespace type_parser {
    class TypeParser {
        private:
            type_lexer lexer;
            token curr;

            void expect(const token::token_type type);
            bool accept(const token::token_type type);
            void consume();

            void throw_parse_err();
        public:
            TypeParser(type_lexer& lexer);
            TypeDefinition parse_next_type();
            bool end();
            ~TypeParser();
    };
}
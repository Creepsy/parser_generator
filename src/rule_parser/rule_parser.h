#pragma once

#include <vector>
#include <string>

#include <rule_lexer.h>

#include "parser_infos.h"

namespace rparser {
    const std::vector<std::string> TOKEN_TYPE_TO_STR{{
        "END_OF_FILE",
        "UNDEFINED",
        "WSPACE",
        "COMMENT",
        "ENTRY_POINT",
        "EOL",
        "PAR_OPEN",
        "PAR_CLOSE",
        "SQ_BRACKET_OPEN",
        "SQ_BRACKET_CLOSE",
        "RETURN",
        "NONE",
        "ASSIGN",
        "SEPERATOR",
        "DEFINE_TYPE",
        "APPEND",
        "VECTOR",
        "TOKEN_TYPE",
        "NAMESPACE",
        "TOKEN",
        "IDENTIFIER"
    }};

    class rule_parser {
        private:
            rule_lexer& lexer;
            token curr;

            void expect(const token::token_type type);
            void expect(const std::vector<token::token_type>& types);
            bool accept(const token::token_type type);
            bool accept(const std::vector<token::token_type>& types);

            void consume();
            void consume(const token::token_type type);

            parser_infos::type_definition parse_type_definition();
            parser_infos::rule parse_rule();
            parser_infos::type_parameter parse_type_parameter();
        public:
            rule_parser(rule_lexer& lexer);
            parser_infos::parser_info parse_parser_infos();
            ~rule_parser();
    };
}
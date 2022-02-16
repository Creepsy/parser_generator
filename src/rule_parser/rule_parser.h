#pragma once

#include <rule_lexer.h>

#include "../util/parser_base.h"

namespace rule_parser {
        class RuleParser : public ParserBase<rule_lexer::rule_lexer, rule_lexer::token> {
        public:
            RuleParser(rule_lexer::rule_lexer& lexer);
            ~RuleParser();
    };    
}
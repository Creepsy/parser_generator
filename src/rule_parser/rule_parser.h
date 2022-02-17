#pragma once

#include <rule_lexer.h>

#include "../util/parser_base.h"
#include "rules.h"

namespace rule_parser {
        class RuleParser : public ParserBase<rule_lexer::rule_lexer, rule_lexer::token> {
            private:
                std::vector<Parameter> parse_parameters();
                RuleResult parse_rule_result();
            public:
                RuleParser(rule_lexer::rule_lexer& lexer);
                RuleDefinition parse_next_rule();
                ~RuleParser();
    };    
}
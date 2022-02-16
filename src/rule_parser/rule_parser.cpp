#include "rule_parser.h"

using namespace rule_parser;

rule_parser::RuleParser::RuleParser(rule_lexer::rule_lexer& lexer) : ParserBase(lexer) {
}

rule_parser::RuleParser::~RuleParser() {
    
}
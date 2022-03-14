#pragma once

#include <ostream>
#include <map>

#include "../parser/parser_generator.h"

#include "../../rule_parser/rules.h"
#include "../../type_parser/types.h"

#include "generation_utils.h"

namespace code_generator {
    typedef std::map<rule_parser::RuleDefinition, size_t> RuleIDMap;

    RuleIDMap generate_rule_ids(const std::vector<rule_parser::RuleDefinition>& rules);
    void generate_parser_header_code(std::ostream& target, const ParserFileInfo& parser_info,
            const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const RuleIDMap& rule_mappings);
    void generate_parser_source_code(std::ostream& target, const ParserFileInfo& parser_info,
            const LexerFileInfo& lexer_info, const parser_generator::StatesInfo& states_info, const RuleIDMap& rule_mappings, const type_parser::TypeInfoTable& type_infos);
}
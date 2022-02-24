#pragma once

#include <map>
#include <cstddef>
#include <vector>

#include "../../rule_parser/rules.h"

#include "../../type_parser/types.h"


#include "states.h"

namespace parser_generator {
    states::State create_start_state(const std::vector<rule_parser::RuleDefinition>& rules);
    size_t process_state(states::State to_process, std::map<states::State, size_t>& processed, const std::vector<rule_parser::RuleDefinition>& rules,
            const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table);

    std::set<rule_parser::RuleDefinition> get_sub_rules(const std::vector<rule_parser::RuleDefinition>& rules, const rule_parser::RuleDefinition& to_check,
            const type_parser::TypeInfoTable& type_infos);
    std::set<rule_parser::RuleDefinition> get_sub_rules(const std::vector<rule_parser::RuleDefinition>& rules, const rule_parser::RuleDefinition& to_check,
            const type_parser::TypeInfoTable& type_infos, std::set<rule_parser::RuleDefinition>& already_checked);
    
    std::map<states::State, size_t> generate_parser_states(const std::vector<rule_parser::RuleDefinition>& rules, const type_parser::TypeInfoTable& type_infos,
            const states::StartTokensTable& start_table);

}
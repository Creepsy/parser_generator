#include "states.h"

#include <algorithm>
#include <iterator>

using namespace states;

states::RuleState::RuleState(rule_parser::RuleDefinition& rule) : rule(rule), position(0) {
}

states::RuleState::~RuleState() {
}

StartTokensTable states::construct_start_token_table(const std::vector<rule_parser::RuleDefinition>& rules, const type_parser::TypeInfoTable& type_infos) {
    StartTokensTable start_tokens;
    
    for(const std::pair<std::string, type_parser::TypeInfo>& type_info : type_infos)
        start_tokens.insert(std::make_pair(type_info.first, std::set<rule_parser::Parameter>{}));

    for(const rule_parser::RuleDefinition& rule : rules) {
        if(!rule.parameters.empty() && !rule.is_entry) 
            start_tokens[rule.result.type].insert(rule.parameters.front()); 
    }   

    for(const std::pair<std::string, type_parser::TypeInfo>& type_info : type_infos) {
        if(type_info.second.is_base) {
            for(const std::string& possible_type : type_info.second.possible_types) {
                std::copy(
                    start_tokens[possible_type].begin(), start_tokens[possible_type].end(),
                    std::inserter(start_tokens[type_info.first], start_tokens[type_info.first].end())
                );
                
            }
        }
    }

    return start_tokens;
}
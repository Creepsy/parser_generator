#include "parser_generator.h"

#include <algorithm>
#include <iterator>

using namespace parser_generator;

//helper functions
void complete_rules(states::State& to_process, const std::vector<rule_parser::RuleDefinition>& rules,
        const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table);
void generate_actions(states::State& to_process, std::map<states::State, size_t>& processed, const std::vector<rule_parser::RuleDefinition>& rules,
        const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table);
std::set<rule_parser::Argument> unwrap_type(const rule_parser::Argument& to_unwrap, const type_parser::TypeInfoTable& type_infos);

void complete_rules(states::State& to_process, const std::vector<rule_parser::RuleDefinition>& rules,
        const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table) {
    std::set<rule_parser::RuleDefinition> rules_to_add;

    for(const states::RuleState& rule_state : to_process.rule_possibilities) {
        rules_to_add.merge(get_sub_rules(rules, rule_state, type_infos));
    }

    std::transform(rules_to_add.begin(), rules_to_add.end(), std::inserter(to_process.rule_possibilities, to_process.rule_possibilities.end()), 
        [](const rule_parser::RuleDefinition& rule) -> states::RuleState { return states::RuleState(rule); }
    );

    for(const rule_parser::RuleDefinition& added_rule : rules_to_add) {
        std::set<rule_parser::Argument> lookahead_tokens = states::get_lookahead_tokens(to_process, added_rule.result.type, start_table, type_infos);
        to_process.rule_possibilities.erase(states::RuleState(added_rule));
        to_process.rule_possibilities.insert(states::RuleState(added_rule, 0, lookahead_tokens));
    }
}

void generate_actions(states::State& to_process, std::map<states::State, size_t>& processed, const std::vector<rule_parser::RuleDefinition>& rules,
        const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table) {
    std::set<rule_parser::Argument> processed_parameters;
    for(const states::RuleState& rule : to_process.rule_possibilities) {
        if(rule.curr().has_value()) {
            for(const rule_parser::Argument& arg : unwrap_type(rule.curr().value(), type_infos)) {
                if(!processed_parameters.contains(arg)) {
                    processed_parameters.insert(arg);

                    size_t sub_state_id = process_state(to_process.advance(arg, type_infos), processed, rules, type_infos, start_table);
                    to_process.actions.insert(states::Action{
                        (arg.is_token) ? states::Action::SHIFT : states::Action::GOTO,
                        sub_state_id,
                        std::set<rule_parser::Argument>{arg}
                    });
                }
            }
        } else {
            to_process.actions.insert(states::Action{
                (rule.get_rule().is_entry) ? states::Action::ACCEPT : states::Action::REDUCE,
                rule.get_rule(),
                rule.get_possible_lookaheads()
            });
        }
    }
}

std::set<rule_parser::Argument> unwrap_type(const rule_parser::Argument& to_unwrap, const type_parser::TypeInfoTable& type_infos) {
    if(to_unwrap.is_token || to_unwrap.is_vector || !type_infos.at(to_unwrap.identifier).is_base)
        return std::set<rule_parser::Argument>{to_unwrap};

    std::set<rule_parser::Argument> unwrapped;
    const std::set<std::string>& possible_types = type_infos.at(to_unwrap.identifier).possible_types; 

    std::transform(possible_types.begin(), possible_types.end(), std::inserter(unwrapped, unwrapped.end()), 
        [&](const std::string& type) -> rule_parser::Argument {
            return rule_parser::Argument{false, false, type, to_unwrap.scope};    
        }
    );

    return unwrapped;
}



states::State parser_generator::create_start_state(const std::vector<rule_parser::RuleDefinition>& rules) {
    states::State start_state;

    for(const rule_parser::RuleDefinition& rule : rules) {
        if(rule.is_entry)
            start_state.rule_possibilities.insert(states::RuleState(rule));
    }

    return start_state;
}

size_t parser_generator::process_state(states::State to_process, std::map<states::State, size_t>& processed, const std::vector<rule_parser::RuleDefinition>& rules,
        const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table) {
    complete_rules(to_process, rules, type_infos, start_table);

    //state already exists
    if(processed.contains(to_process)) {
        return processed[to_process];
    }

    generate_actions(to_process, processed, rules, type_infos, start_table);

    processed.insert(std::make_pair(to_process, processed.size()));

    return processed.size() - 1;
}

std::set<rule_parser::RuleDefinition> parser_generator::get_sub_rules(const std::vector<rule_parser::RuleDefinition>& rules, const states::RuleState& to_check,
        const type_parser::TypeInfoTable& type_infos) {
    std::set<rule_parser::RuleDefinition> already_checked;

    return get_sub_rules(rules, to_check, type_infos, already_checked);
}

std::set<rule_parser::RuleDefinition> parser_generator::get_sub_rules(const std::vector<rule_parser::RuleDefinition>& rules, const states::RuleState& to_check,
        const type_parser::TypeInfoTable& type_infos, std::set<rule_parser::RuleDefinition>& already_checked) {
    std::set<rule_parser::RuleDefinition> sub_rules;

    already_checked.insert(to_check.get_rule());

    if(to_check.curr().has_value() && !to_check.curr().value().is_token) {
        for(const rule_parser::RuleDefinition& rule : rules) {
            if(!rule.is_entry && !already_checked.contains(rule)) {
                if(type_parser::is_convertible(rule.result.type.identifier, to_check.curr().value().identifier, type_infos)) {
                    if(rule.result.type.is_vector == to_check.curr().value().is_vector && rule.result.type.scope == to_check.curr().value().scope) {
                        if(!rule.result.type.is_vector || rule.result.type.identifier == to_check.curr().value().identifier) {
                            sub_rules.insert(rule);
                            sub_rules.merge(get_sub_rules(rules, states::RuleState(rule), type_infos, already_checked));
                        }
                    }
                }
            }
        }
    }

    return sub_rules;
}

StatesInfo parser_generator::generate_parser_states(const std::vector<rule_parser::RuleDefinition>& rules,
        const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table) {
    std::map<states::State, size_t> parser_states;
    size_t start_state = process_state(create_start_state(rules), parser_states, rules, type_infos, start_table);

    return StatesInfo{parser_states, start_state};
}
#include "parser_generator.h"

#include <algorithm>
#include <iterator>

using namespace parser_generator;

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
    std::set<rule_parser::RuleDefinition> rules_to_add;

    for(const states::RuleState& rule_state : to_process.rule_possibilities) {
        rules_to_add.merge(get_sub_rules(rules, rule_state.get_rule(), type_infos));
    }

    std::transform(rules_to_add.begin(), rules_to_add.end(), std::inserter(to_process.rule_possibilities, to_process.rule_possibilities.end()), 
        [](const rule_parser::RuleDefinition& rule) -> states::RuleState { return states::RuleState(rule); }
    );

    for(const rule_parser::RuleDefinition& added_rule : rules_to_add) {
        to_process.rule_possibilities.erase(states::RuleState(added_rule));
        to_process.rule_possibilities.insert(
            states::RuleState(added_rule, 0, states::get_lookahead_tokens(to_process, added_rule.result.type, start_table, type_infos))
        );
    }

    //state already exists
    if(processed.contains(to_process)) {
        return processed[to_process];
    }

    std::set<rule_parser::Parameter> processed_parameters;
    for(const states::RuleState& rule : to_process.rule_possibilities) {
        if(rule.curr().has_value()) {
            if(!processed_parameters.contains(rule.curr().value())) {
                const rule_parser::Parameter curr = rule.curr().value();
                processed_parameters.insert(curr);

                size_t sub_state_id = process_state(to_process.advance(curr), processed, rules, type_infos, start_table);
                to_process.actions.insert(states::Action{
                    (curr.is_token) ? states::Action::SHIFT : states::Action::GOTO,
                    sub_state_id,
                    rule.get_possible_lookaheads()
                });
            }
        } else {
            to_process.actions.insert(states::Action{
                (rule.get_rule().is_entry) ? states::Action::ACCEPT : states::Action::REDUCE,
                rule.get_rule(),
                rule.get_possible_lookaheads()
            });
        }
    }

    processed.insert(std::make_pair(to_process, processed.size()));

    return processed.size() - 1;
}

std::set<rule_parser::RuleDefinition> parser_generator::get_sub_rules(const std::vector<rule_parser::RuleDefinition>& rules, const rule_parser::RuleDefinition& to_check,
        const type_parser::TypeInfoTable& type_infos) {
    std::set<rule_parser::RuleDefinition> already_checked;

    return get_sub_rules(rules, to_check, type_infos, already_checked);
}

std::set<rule_parser::RuleDefinition> parser_generator::get_sub_rules(const std::vector<rule_parser::RuleDefinition>& rules, const rule_parser::RuleDefinition& to_check,
        const type_parser::TypeInfoTable& type_infos, std::set<rule_parser::RuleDefinition>& already_checked) {
    std::set<rule_parser::RuleDefinition> sub_rules;

    already_checked.insert(to_check);

    if(!to_check.parameters.empty() && !to_check.parameters.front().is_token) {
        for(const rule_parser::RuleDefinition& rule : rules) {
            if(!rule.is_entry && !already_checked.contains(rule)) {
                if(type_parser::is_convertible(rule.result.type, to_check.parameters.front().identifier, type_infos)) {
                    sub_rules.insert(rule);
                    sub_rules.merge(get_sub_rules(rules, rule, type_infos, already_checked));
                }
            }
        }
    }

    return sub_rules;
}

std::map<states::State, size_t> parser_generator::generate_parser_states(const std::vector<rule_parser::RuleDefinition>& rules,
        const type_parser::TypeInfoTable& type_infos, const states::StartTokensTable& start_table) {
    std::map<states::State, size_t> parser_states;
    process_state(create_start_state(rules), parser_states, rules, type_infos, start_table);

    return parser_states;
}
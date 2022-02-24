#include "states.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include "../../util/comparators.h"

using namespace states;

states::RuleState::RuleState(const rule_parser::RuleDefinition& rule) : rule(rule), position(0) {
}

states::RuleState::RuleState(const rule_parser::RuleDefinition& rule, const size_t position, const std::set<std::string>& possible_lookaheads)
 : rule(rule), position(position), possible_lookaheads(possible_lookaheads) {

}

std::optional<RuleState> states::RuleState::advance(const rule_parser::Parameter& to_expect) const {
    return (this->curr() == to_expect) ? std::optional<RuleState>(RuleState(this->rule, this->position + 1, this->possible_lookaheads)) : std::nullopt;
}

std::optional<rule_parser::Parameter> states::RuleState::get(const size_t position) const {
    return (position >= this->rule.parameters.size()) ? std::nullopt : std::optional<rule_parser::Parameter>(this->rule.parameters.at(position));
}

std::optional<rule_parser::Parameter> states::RuleState::curr() const {
    return this->get(this->position);
}

std::optional<rule_parser::Parameter> states::RuleState::lookahead() const {
    return this->get(this->position + 1);
}

const rule_parser::RuleDefinition& states::RuleState::get_rule() const {
    return this->rule;
}

const std::set<std::string> states::RuleState::get_possible_lookaheads() const {
    return this->possible_lookaheads;
}

bool states::RuleState::end() const {
    return this->position >= this->rule.parameters.size();
}

states::RuleState::~RuleState() {
}



State states::State::advance(const rule_parser::Parameter& to_expect) const {
    State new_state;

    for(const RuleState& rule : this->rule_possibilities) {
        std::optional<RuleState> advanced_rule = rule.advance(to_expect);
        if(advanced_rule.has_value())
            new_state.rule_possibilities.insert(advanced_rule.value());
    }

    return new_state;
}



StartTokensTable states::construct_start_token_table(const std::vector<rule_parser::RuleDefinition>& rules, const type_parser::TypeInfoTable& type_infos) {
    StartTokensTable start_table;
    
    for(const std::pair<std::string, type_parser::TypeInfo>& type_info : type_infos)
        start_table.insert(std::make_pair(type_info.first, std::set<rule_parser::Parameter>{}));

    for(const rule_parser::RuleDefinition& rule : rules) {
        if(!rule.parameters.empty() && !rule.is_entry) 
            start_table[rule.result.type].insert(rule.parameters.front()); 
    }   

    for(const std::pair<std::string, type_parser::TypeInfo>& type_info : type_infos) {
        if(type_info.second.is_base) {
            for(const std::string& possible_type : type_info.second.possible_types) {
                std::copy(
                    start_table[possible_type].begin(), start_table[possible_type].end(),
                    std::inserter(start_table[type_info.first], start_table[type_info.first].end())
                );
                
            }
        }
    }

    return start_table;
}

std::set<std::string> states::get_start_tokens(const std::string& type, const StartTokensTable& start_table) {
    std::set<std::string> visited_types;

    return get_start_tokens(type, start_table, visited_types);
}

std::set<std::string> states::get_start_tokens(const std::string& type, const StartTokensTable& start_table, std::set<std::string>& visited_types) {
    std::set<std::string> start_tokens;

    visited_types.insert(type);

    for(const rule_parser::Parameter& possible_start : start_table.at(type)) {
        if(possible_start.is_token) {
            start_tokens.insert(possible_start.identifier);
        } else {
            if(!visited_types.contains(possible_start.identifier)) {
                start_tokens.merge(get_start_tokens(possible_start.identifier, start_table, visited_types));
            }
        }
    }

    return start_tokens;
}

std::set<std::string> states::get_lookahead_tokens(const State& state, const std::string& type, const StartTokensTable& start_table, const type_parser::TypeInfoTable& type_infos) {
    std::set<std::string> follow_up_tokens;

    for(const RuleState& rule : state.rule_possibilities) {
        std::optional<rule_parser::Parameter> curr = rule.curr();
        std::optional<rule_parser::Parameter> follow_up = rule.lookahead();

        if(curr.has_value() && follow_up.has_value()) {
            if(!curr.value().is_token && type_parser::is_convertible(type, curr.value().identifier, type_infos)) {
                if(follow_up.value().is_token) {
                    follow_up_tokens.insert(follow_up.value().identifier);
                } else {
                    follow_up_tokens.merge(get_start_tokens(follow_up.value().identifier, start_table));
                }
            }
        }
    }

    return follow_up_tokens;
}



std::ostream& states::operator<<(std::ostream& stream, const RuleState& to_write) {
    if(to_write.get_rule().is_entry) stream << "* ";
    
    for(size_t pos = 0; pos < to_write.get_rule().parameters.size(); pos++) {
        if(pos == to_write.position)
            stream << "|";
        stream << to_write.get_rule().parameters.at(pos) << " ";
    }

    return stream << ": " << to_write.get_rule().result << ";";
}

std::ostream& states::operator<<(std::ostream& stream, const Action& to_write) {
    const static std::vector<std::string> ACTION_TYPE_STRINGS = {"SHIFT", "GOTO", "REDUCE", "ACCEPT"};

    stream << ACTION_TYPE_STRINGS[to_write.type] << " ";
    
    std::visit([&](const auto& to_print) -> void { stream << to_print; }, to_write.operand);

    stream << " on ";

    for(const std::string& lookahead : to_write.possible_lookaheads)
        stream << lookahead << " ";

    return stream;
}

std::ostream& states::operator<<(std::ostream& stream, const State& to_write) {
    stream << "State:\n\tRules:\n";

    for(const RuleState& rule : to_write.rule_possibilities)
        stream << "\t\t" << rule << "\n";

    stream << "\tActions:\n";

    for(const Action& action : to_write.actions) 
        stream << "\t\t" << action << "\n";

    return stream;
}



bool states::operator<(const RuleState& first, const RuleState& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.rule, second.rule),
        std::make_pair(first.position, second.position),
        std::make_pair(first.possible_lookaheads, second.possible_lookaheads)
    );
}

bool states::operator<(const Action& first, const Action& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.type, second.type),
        std::make_pair(first.operand, second.operand),
        std::make_pair(first.possible_lookaheads, second.possible_lookaheads)  
    );
}

bool states::operator<(const State& first, const State& second) {
    return first.rule_possibilities < second.rule_possibilities;
}
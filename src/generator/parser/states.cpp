#include "states.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include "../../util/comparators.h"

using namespace states;

states::RuleState::RuleState(const rule_parser::RuleDefinition& rule) : rule(rule), position(0) {
}

states::RuleState::RuleState(const rule_parser::RuleDefinition& rule, const size_t position, const std::set<rule_parser::Argument>& possible_lookaheads)
 : rule(rule), position(position), possible_lookaheads(possible_lookaheads) {

}

std::optional<RuleState> states::RuleState::advance(const rule_parser::Argument& to_expect, const type_parser::TypeInfoTable& type_infos) const {
    if(this->end()) 
        return std::nullopt;

    std::string from = (to_expect.is_token) ? "Token" : to_expect.identifier;
    std::string to = (this->curr().value().is_token) ? "Token" : this->curr().value().identifier;

    if(from == "Token" && to == "Token" && to_expect.identifier != this->curr().value().identifier)
        return std::nullopt;

    if(to_expect.is_vector != this->curr().value().is_vector)
        return std::nullopt;

    if(to_expect.is_vector && to_expect.identifier != this->curr().value().identifier)
        return std::nullopt;

    return (type_parser::is_convertible(from, to, type_infos)) ? std::optional<RuleState>(RuleState(this->rule, this->position + 1, this->possible_lookaheads)) : std::nullopt;
}

std::optional<rule_parser::Argument> states::RuleState::get(const size_t position) const {
    return (position >= this->rule.arguments.size()) ? std::nullopt : std::optional<rule_parser::Argument>(this->rule.arguments.at(position));
}

std::optional<rule_parser::Argument> states::RuleState::curr() const {
    return this->get(this->position);
}

std::optional<rule_parser::Argument> states::RuleState::lookahead() const {
    return this->get(this->position + 1);
}

const rule_parser::RuleDefinition& states::RuleState::get_rule() const {
    return this->rule;
}

const std::set<rule_parser::Argument>& states::RuleState::get_possible_lookaheads() const {
    return this->possible_lookaheads;
}

bool states::RuleState::end() const {
    return !this->curr().has_value();
}

states::RuleState::~RuleState() {
}



State states::State::advance(const rule_parser::Argument& to_expect, const type_parser::TypeInfoTable& type_infos) const {
    State new_state;

    for(const RuleState& rule : this->rule_possibilities) {
        std::optional<RuleState> advanced_rule = rule.advance(to_expect, type_infos);
        if(advanced_rule.has_value())
            new_state.rule_possibilities.insert(advanced_rule.value());
    }

    return new_state;
}



StartTokensTable states::construct_start_token_table(const std::vector<rule_parser::RuleDefinition>& rules, const type_parser::TypeInfoTable& type_infos) {
    StartTokensTable start_table;

    auto copy_entry = [&](const rule_parser::Argument& from, const rule_parser::Argument& to) -> void {
        std::copy(
            start_table[from].begin(), start_table[from].end(),
            std::inserter(start_table[to], start_table[to].end())
        );
    };

    //For Token[]
    start_table.insert(std::make_pair(rule_parser::Argument{false, true, "Token"}, std::set<rule_parser::Argument>{}));

    for(const std::pair<std::string, type_parser::TypeInfo>& type_info : type_infos) {
        start_table.insert(std::make_pair(rule_parser::Argument{false, false, type_info.first}, std::set<rule_parser::Argument>{}));
        start_table.insert(std::make_pair(rule_parser::Argument{false, true, type_info.first}, std::set<rule_parser::Argument>{}));
    }

    for(const rule_parser::RuleDefinition& rule : rules) {
        if(!rule.arguments.empty() && !rule.is_entry) 
            start_table[rule.result.type].insert(rule.arguments.front()); 
    }   

    for(const std::pair<std::string, type_parser::TypeInfo>& type_info : type_infos) {
        if(type_info.second.is_base) {
            const rule_parser::Argument base_type{false, false, type_info.first};
            const rule_parser::Argument base_type_vec{false, true, type_info.first};

            for(const std::string& possible_type_str : type_info.second.possible_types) {
                copy_entry(rule_parser::Argument{false, false, possible_type_str}, base_type);
                copy_entry(rule_parser::Argument{false, true, possible_type_str}, base_type_vec);
            }
        }
    }

    return start_table;
}

std::set<rule_parser::Argument> states::get_start_tokens(const rule_parser::Argument& type, const StartTokensTable& start_table) {
    std::set<rule_parser::Argument> visited_types;

    return get_start_tokens(type, start_table, visited_types);
}

std::set<rule_parser::Argument> states::get_start_tokens(const rule_parser::Argument& type, const StartTokensTable& start_table, std::set<rule_parser::Argument>& visited_types) {
    std::set<rule_parser::Argument> start_tokens;

    visited_types.insert(type);

    for(const rule_parser::Argument& possible_start : start_table.at(type)) {
        if(possible_start.is_token) {
            start_tokens.insert(possible_start);
        } else {
            if(!visited_types.contains(possible_start)) {
                start_tokens.merge(get_start_tokens(possible_start, start_table, visited_types));
            }
        }
    }

    return start_tokens;
}

std::set<rule_parser::Argument> states::get_lookahead_tokens(const State& state, const rule_parser::Argument& type, const StartTokensTable& start_table, const type_parser::TypeInfoTable& type_infos) {
    std::set<rule_parser::Argument> follow_up_tokens;

    for(const RuleState& rule : state.rule_possibilities) {
        std::optional<rule_parser::Argument> curr = rule.curr();
        std::optional<rule_parser::Argument> follow_up = rule.lookahead();

        if(curr.has_value()) {
            if(!curr.value().is_token && type_parser::is_convertible(type.identifier, curr.value().identifier, type_infos) && type.is_vector == curr.value().is_vector) {
                if(follow_up.has_value()) {
                    if(follow_up.value().is_token) {
                        follow_up_tokens.insert(rule_parser::Argument{false, false, follow_up.value().identifier});
                    } else {
                        follow_up_tokens.merge(get_start_tokens(follow_up.value(), start_table));
                    }
                } else {
                    std::copy(rule.get_possible_lookaheads().begin(), rule.get_possible_lookaheads().end(), std::inserter(follow_up_tokens, follow_up_tokens.end()));
                }
            }
        }
    }

    return follow_up_tokens;
}



std::ostream& states::operator<<(std::ostream& stream, const RuleState& to_write) {
    stream << "<<< ";
    for(const rule_parser::Argument& lookahead : to_write.get_possible_lookaheads()) 
        stream << lookahead << " ";

    stream << ">>> ";
    if(to_write.get_rule().is_entry) stream << "* ";
    
    for(size_t pos = 0; pos < to_write.get_rule().arguments.size(); pos++) {
        if(pos == to_write.position)
            stream << "|";
        stream << to_write.get_rule().arguments.at(pos) << " ";
    }

    if(to_write.end())
        stream << "|";

    return stream << ": " << to_write.get_rule().result << ";";
}

std::ostream& states::operator<<(std::ostream& stream, const Action& to_write) {
    const static std::vector<std::string> ACTION_TYPE_STRINGS = {"SHIFT", "GOTO", "REDUCE", "ACCEPT"};

    stream << ACTION_TYPE_STRINGS[to_write.type] << " ";
    
    std::visit([&](const auto& to_print) -> void { stream << to_print; }, to_write.operand);

    stream << " on ";

    for(const rule_parser::Argument& lookahead : to_write.possible_lookaheads)
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
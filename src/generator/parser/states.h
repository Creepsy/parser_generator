#pragma once

#include <set>
#include <string>
#include <cstddef>
#include <map>
#include <optional>
#include <cstddef>
#include <variant>
#include <ostream>
#include <vector>

#include "../../rule_parser/rules.h"

#include "../../type_parser/types.h"

namespace states {
    //Key: TypeName, Value: possible start tokens
    typedef std::map<std::string, std::set<rule_parser::Parameter>> StartTokensTable;

    class RuleState {
        private:
            const rule_parser::RuleDefinition rule;
            size_t position;

            std::set<std::string> possible_lookaheads;
        public:
            RuleState(const rule_parser::RuleDefinition& rule);
            RuleState(const rule_parser::RuleDefinition& rule, const size_t position, const std::set<std::string>& possible_lookaheads);
            std::optional<RuleState> advance(const rule_parser::Parameter& to_expect) const;
            std::optional<rule_parser::Parameter> get(const size_t position) const;
            std::optional<rule_parser::Parameter> curr() const;
            std::optional<rule_parser::Parameter> lookahead() const;
            const rule_parser::RuleDefinition& get_rule() const;
            const std::set<std::string> get_possible_lookaheads() const;
            bool end() const;
            ~RuleState();

        friend std::ostream& operator<<(std::ostream& stream, const RuleState& to_write);
        friend bool operator<(const RuleState& first, const RuleState& second);
    };

    struct Action {
        enum ActionType {
            SHIFT,
            GOTO,
            REDUCE,
            ACCEPT
        };

        ActionType type;
        std::variant<rule_parser::RuleDefinition, size_t> operand; //RuleDefinition for REDUCE / ACCEPT; size_t for GOTO / SHIFT
        std::set<std::string> possible_lookaheads;
    };

    struct State {
        std::set<RuleState> rule_possibilities;
        std::set<Action> actions;

        State advance(const rule_parser::Parameter& to_expect) const;
    };
    
    StartTokensTable construct_start_token_table(const std::vector<rule_parser::RuleDefinition>& rules, const type_parser::TypeInfoTable& type_infos);
    
    std::set<std::string> get_start_tokens(const std::string& type, const StartTokensTable& start_table);
    std::set<std::string> get_start_tokens(const std::string& type, const StartTokensTable& start_table, std::set<std::string>& visited_types);

    std::set<std::string> get_lookahead_tokens(const State& state, const std::string& type, const StartTokensTable& start_table, const type_parser::TypeInfoTable& type_infos);

    std::ostream& operator<<(std::ostream& stream, const RuleState& to_write);
    std::ostream& operator<<(std::ostream& stream, const Action& to_write);
    std::ostream& operator<<(std::ostream& stream, const State& to_write);

    //for std-containers
    bool operator<(const RuleState& first, const RuleState& second);
    bool operator<(const Action& first, const Action& second);
    bool operator<(const State& first, const State& second);
}
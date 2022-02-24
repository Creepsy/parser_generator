#pragma once

#include <set>
#include <string>
#include <cstddef>
#include <map>
#include <optional>
#include <cstddef>

#include "../../rule_parser/rules.h"

#include "../../type_parser/types.h"

namespace states {
    //Key: TypeName, Value: possible start tokens
    typedef std::map<std::string, std::set<rule_parser::Parameter>> StartTokensTable;

    class RuleState {
        private:
            const rule_parser::RuleDefinition& rule;
            size_t position;

            std::set<std::string> possible_lookaheads;
        public:
            RuleState(const rule_parser::RuleDefinition& rule);
            RuleState(const rule_parser::RuleDefinition& rule, const size_t position, const std::set<std::string>& possible_lookaheads);
            void set_possible_lookaheads(const std::set<std::string>& tokens);
            std::optional<RuleState> advance(const rule_parser::Parameter& to_expect) const;
            std::optional<rule_parser::Parameter> get(const size_t position) const;
            std::optional<rule_parser::Parameter> curr() const;
            std::optional<rule_parser::Parameter> lookahead() const;
            bool end() const;
            ~RuleState();

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
        size_t object_index;
        std::set<std::string> possible_types;
    };

    struct State {
        std::set<RuleState> rule_possibilities;
        std::set<Action> actions;
    };
    
    StartTokensTable construct_start_token_table(const std::vector<rule_parser::RuleDefinition>& rules, const type_parser::TypeInfoTable& type_infos);
    
    std::set<std::string> get_start_tokens(const std::string& type, const StartTokensTable& start_table);
    std::set<std::string> get_start_tokens(const std::string& type, const StartTokensTable& start_table, std::set<std::string>& visited_types);

    std::set<std::string> get_follow_up_tokens(const State& state, const std::string& type, const StartTokensTable& start_table, const type_parser::TypeInfoTable& type_infos);

    //for sets
    bool operator<(const RuleState& first, const RuleState& second);
}
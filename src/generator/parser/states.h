#pragma once

#include <set>
#include <string>
#include <cstddef>
#include <map>

#include "../../rule_parser/rules.h"

#include "../../type_parser/types.h"

namespace states {
    //Key: TypeName, Value: possible start tokens
    typedef std::map<std::string, std::set<rule_parser::Parameter>> StartTokensTable;

    class RuleState {
        private:
            rule_parser::RuleDefinition& rule;
            size_t position;

            std::set<std::string> possible_lookaheads;
        public:
            RuleState(rule_parser::RuleDefinition& rule);
            ~RuleState();
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
        std::set<RuleState> possibilities;
        std::set<Action> actions;
    };
    
    StartTokensTable construct_start_token_table(const std::vector<rule_parser::RuleDefinition>& rules, const type_parser::TypeInfoTable& type_infos);
}
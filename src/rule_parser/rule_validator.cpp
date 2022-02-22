#include "rule_validator.h"

#include <stdexcept>
#include <set>
#include <sstream>

using namespace rule_parser;

void rule_parser::validate_rules(const std::vector<RuleDefinition>& to_validate, const type_parser::TypeInfoTable& type_infos) {
    std::set<RuleDefinition> validated_rules;

    for(const RuleDefinition& rule : to_validate) {
        validate_rule(rule, type_infos);

        if(validated_rules.contains(rule)) {
            std::stringstream rule_str;
            rule_str << rule;

            throw std::runtime_error("Multiple definition of rule '" + rule_str.str() + "'!");
        }

        validated_rules.insert(rule);
    }
}

void rule_parser::validate_rule(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos) {
}
#pragma once

#include "../type_parser/types.h"

#include "rules.h"

namespace rule_parser {
    void validate_rules(const std::vector<RuleDefinition>& to_validate, const type_parser::TypeInfoTable& type_infos);
    void validate_rule(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos);
}
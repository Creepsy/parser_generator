#include "rule_validation.h"

#include <set>

#include "validation_utils.h"

using namespace validation;

void validate_rule(const parser_infos::rule& to_validate);
void check_for_doubled_parameters(const parser_infos::rule& to_validate);
//void validate_parameters(); TODO
//void validate_rule_result(); TODO

void validate_rule(const parser_infos::rule& to_validate) {
    check_for_doubled_parameters(to_validate);
}

void check_for_doubled_parameters(const parser_infos::rule& to_validate) {
    std::set<std::string> used_identifiers;

    for(const parser_infos::type_parameter& par : to_validate.parameters) {
        if(!par.identifer.empty()) {
            if(used_identifiers.find(par.identifer) != used_identifiers.end())
                throw_validation_error("Same identifier '" + par.identifer + "' for multiple parameters in the same rule!");
            used_identifiers.insert(par.identifer);
        }
    }
}



void validation::validate_rules(const parser_infos::parser_info& to_validate) {
    for(const parser_infos::rule& r : to_validate.entry_rules) {
        validate_rule(r);
    }

    for(const parser_infos::rule& r : to_validate.parser_rules) {
        validate_rule(r);
    }
}
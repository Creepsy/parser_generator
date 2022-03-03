#include "rule_validator.h"

#include <stdexcept>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>

using namespace rule_parser;

//helper functions
void validate_rule_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos);
void validate_result_argument(const RuleDefinition& to_validate, const size_t arg, const type_parser::TypeInfoTable& type_infos);

void validate_rule_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos) {
    if(to_validate.result.type.identifier.empty())
        throw std::runtime_error("The rule is missing a return type!");

    if(to_validate.result.type.identifier == "Token") 
        throw std::runtime_error("The result of a rule can't be a token!");

    if(!type_infos.contains(to_validate.result.type.identifier))
        throw std::runtime_error("The type '" + to_validate.result.type.identifier + "' doesn't exist!");

    if(to_validate.result.result_type == RuleResult::CREATE_NEW) {
        const type_parser::TypeInfo& return_type_info = type_infos.at(to_validate.result.type.identifier);

        if(return_type_info.is_base) 
            throw std::runtime_error("Base types can't be constructed!");

        if(return_type_info.parameters.size() != to_validate.result.argument_ids.size()) 
            throw std::runtime_error("Mismatched argument count!");

        for(size_t arg = 0; arg < to_validate.result.argument_ids.size(); arg++) 
            validate_result_argument(to_validate, arg, type_infos);   
    }
}

void validate_result_argument(const RuleDefinition& to_validate, const size_t arg, const type_parser::TypeInfoTable& type_infos) {
    const type_parser::TypeInfo& return_type_info = type_infos.at(to_validate.result.type.identifier);
    const std::optional<size_t>& index = to_validate.result.argument_ids[arg];

    if(!index.has_value()) {
        if(return_type_info.parameters.at(arg).type == "Token") 
            throw std::runtime_error("Tokens can't be default constructed!");
    } else {
        const std::string& expected_type = return_type_info.parameters.at(arg).type;
        const std::string& actual_type = (to_validate.arguments[index.value()].is_token) ? "Token" : to_validate.arguments[index.value()].identifier;

        if(!type_parser::is_convertible(actual_type, expected_type, type_infos))
            throw std::runtime_error("Expected type '" + expected_type + ", found '" + actual_type + "'!");
    }   
}



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
    for(const Argument& arg : to_validate.arguments) {
        if(!arg.is_token && !type_infos.contains(arg.identifier)) 
            throw std::runtime_error("The type '" + arg.identifier + "' doesn't exist!");
    }

    for(const std::optional<size_t>& index : to_validate.result.argument_ids) {
        if(index.has_value() && index.value() >= to_validate.arguments.size()) 
            throw std::runtime_error("Index '" + std::to_string(index.value()) + "' out of bounds!");
    }

    validate_rule_result(to_validate, type_infos);
}
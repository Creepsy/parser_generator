#include "rule_validator.h"

#include <stdexcept>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>

using namespace rule_parser;

//helper functions
void validate_rule_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos);
void validate_create_new_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos);
void validate_create_vector_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos);
void validate_result_argument(const RuleDefinition& to_validate, const size_t arg, const type_parser::TypeInfoTable& type_infos);

void validate_rule_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos) {
    if(to_validate.result.type.identifier.empty())
        throw std::runtime_error("The rule is missing a return type!");

    if(to_validate.is_entry && !to_validate.result.type.scope.empty())
        throw std::runtime_error("Entry rules aren't allowed to have a scope!");

    if(!type_infos.contains(to_validate.result.type.identifier) && !to_validate.result.type.is_token)
        throw std::runtime_error("The type '" + to_validate.result.type.identifier + "' doesn't exist!");

    switch(to_validate.result.result_type) {
        case RuleResult::PASS_ARG:
            if(to_validate.result.type.is_token && !to_validate.result.type.is_vector)
                throw std::runtime_error("The result of a rule can't be a token!");
            break;
        case RuleResult::CREATE_NEW:
            validate_create_new_result(to_validate, type_infos);
            break;
        case RuleResult::CREATE_VECTOR:
            validate_create_vector_result(to_validate, type_infos);
            break;
        default:
            throw std::runtime_error("Invalid rule result type!");
    }
}

void validate_create_new_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos) {
    if(to_validate.result.type.is_token)
        throw std::runtime_error("Invalid construction of a token!");

    if(to_validate.result.type.is_vector)
        throw std::runtime_error("Invalid construction of a vector!");

    const type_parser::TypeInfo& return_type_info = type_infos.at(to_validate.result.type.identifier);

    if(return_type_info.is_base) 
        throw std::runtime_error("Base types can't be constructed!");

    if(return_type_info.parameters.size() != to_validate.result.argument_ids.size()) 
        throw std::runtime_error("Mismatched argument count!");

    for(size_t arg = 0; arg < to_validate.result.argument_ids.size(); arg++) 
        validate_result_argument(to_validate, arg, type_infos);
}

void validate_create_vector_result(const RuleDefinition& to_validate, const type_parser::TypeInfoTable& type_infos) {
    const std::string expected_type = to_validate.result.type.identifier;

    for(const std::optional<size_t>& index : to_validate.result.argument_ids) {
        if(!index.has_value())
            throw std::runtime_error("EMPTY in vector constructors is not allowed!");

        const std::string& actual_type = (to_validate.arguments[index.value()].is_token) ? "Token" : to_validate.arguments[index.value()].identifier;

        if(!type_parser::is_convertible(actual_type, expected_type, type_infos))
            throw std::runtime_error("Expected type '" + expected_type + "', found '" + actual_type + "'!");
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

        if(!return_type_info.parameters.at(arg).is_vector && to_validate.arguments[index.value()].is_vector)
            throw std::runtime_error("Can't convert a vector to a normal type!");

        if(!type_parser::is_convertible(actual_type, expected_type, type_infos))
            throw std::runtime_error("Expected type '" + expected_type + "', found '" + actual_type + "'!");
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
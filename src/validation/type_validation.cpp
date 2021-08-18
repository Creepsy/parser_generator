#include "type_validation.h"

using namespace validation;

#include <vector>
#include <set>
#include <string>
#include <stdexcept>

#include "../util.h"
#include "validation_utils.h"

//helper functions

void validate_type(const parser_infos::type_definition& to_validate, const std::set<std::string>& base_types, const std::set<std::string>& types);
void check_type_members(const parser_infos::type_definition& to_validate, const std::set<std::string> base_types, const std::set<std::string>& types);
void check_for_doubled_types(const std::vector<parser_infos::type_definition>& types);
void validate_base_types(const std::set<std::string>& base_types);
void check_for_reserved_types(const std::string& type_name);

void validate_type(const parser_infos::type_definition& to_validate, const std::set<std::string>& base_types, const std::set<std::string>& types) {
    if(base_types.find(to_validate.type_name) != base_types.end()) 
        throw_validation_error("The type '" + to_validate.type_name + "' is already a base type!");

    check_for_reserved_types(to_validate.type_name);
    check_type_members(to_validate, base_types, types);
}

void check_type_members(const parser_infos::type_definition& to_validate, const std::set<std::string> base_types, const std::set<std::string>& types) {
    std::set<std::string> used_identifiers;

    for(const parser_infos::type_parameter& par : to_validate.members) {
        if(par.type == parser_infos::type_parameter::parameter_type::SPECIFIC_TOKEN) 
            throw_validation_error("Specific tokens aren't allowed as member types!");

        if(par.type == parser_infos::type_parameter::parameter_type::CUSTOM_TYPE) {
            check_for_reserved_types(par.type_identifier);
            
            if(base_types.find(par.type_identifier) == base_types.end() && types.find(par.type_identifier) == types.end())
                throw_validation_error("The type '" + par.type_identifier + "' is used as a member type but doesn't exist!");
        }

        if(par.identifer.empty())
            throw_validation_error("Every type member needs a name!");

        if(used_identifiers.find(par.identifer) != used_identifiers.end())
            throw_validation_error("Multiple members have the same identifier '" + par.identifer + "'!");
        used_identifiers.insert(par.identifer);
    }
}

void check_for_doubled_types(const std::vector<parser_infos::type_definition>& types) {
    std::set<std::string> type_names;

    for(const parser_infos::type_definition& type : types) {
        if(type_names.find(type.type_name) != type_names.end())
            throw_validation_error("Multiple definition of type '" + type.type_name + "'!");
        type_names.insert(type.type_name);
    }
}

void validate_base_types(const std::set<std::string>& base_types) {
    for(const std::string& base_type : base_types) {
        check_for_reserved_types(base_type);
    }
}

void check_for_reserved_types(const std::string& type_name) {
    if(type_name == util::BASE_NODE_NAME || type_name == util::TOKEN_NODE_NAME)
        throw_validation_error("The type '" + type_name + "' is a reserved type which can't be used!");
}



void validation::validate_types(const parser_infos::parser_info& to_validate) {
    const std::set<std::string> base_types = to_validate.get_base_type_names();
    const std::set<std::string> types = to_validate.get_type_names();

    check_for_doubled_types(to_validate.types);
    validate_base_types(base_types);

    for(const parser_infos::type_definition& type : to_validate.types) {
        validate_type(type, base_types, types);
    }
}
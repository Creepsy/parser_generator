#include "type_validator.h"

#include <map>
#include <string>
#include <stdexcept>

using namespace type_parser;

//helper functions
void update_type_map(std::map<std::string, bool>& to_update, const std::string& new_type, const bool is_base_type = false);

void update_type_map(std::map<std::string, bool>& to_update, const std::string& new_type, const bool is_base_type) {
    if(to_update.contains(new_type)) {
        if(!is_base_type || to_update[new_type] != is_base_type) {
            throw std::runtime_error("Multiple definition of type '" + new_type + "'!");
        }
    } 

    to_update.insert(std::make_pair(new_type, is_base_type));
}



void type_parser::validate_types(const std::vector<TypeDefinition>& to_validate) {
    //type name, whether it is a base type
    std::map<std::string, bool> valid_types;

    TypeInfoTable type_infos = construct_type_info_table(to_validate);

    for(const TypeDefinition& type : to_validate) {
        update_type_map(valid_types, type.identifier);

        validate_type(type, type_infos);

        for(const std::string& base_type : type.base_types) update_type_map(valid_types, base_type, true);
    }

    if(valid_types.contains("Token"))
        throw std::runtime_error("The type 'Token' is a reserved type!");
}

void type_parser::validate_type(const TypeDefinition& to_validate, const TypeInfoTable& type_infos) {
    for(const Parameter& par : to_validate.parameters) {
        if(par.second != "Token" && !type_infos.contains(par.second)) throw std::runtime_error("Use of unknown type '" + par.second + "'!");
    }
}
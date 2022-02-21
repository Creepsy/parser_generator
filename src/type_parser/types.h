#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <utility>
#include <map>
#include <set>

namespace type_parser {
    //identifier, type string
    typedef std::pair<std::string, std::string> Parameter;

    struct TypeDefinition {
        std::string identifier;
        
        std::set<std::string> base_types;
        std::vector<Parameter> parameters;
    };

    struct TypeInfo {
        std::set<std::string> possible_types;
        bool is_base;
        std::vector<Parameter> parameters;
    };

    typedef std::map<std::string, TypeInfo> TypeInfoTable;

    TypeInfoTable construct_type_info_table(const std::vector<TypeDefinition>& types);

    std::ostream& operator<<(std::ostream& stream, const TypeDefinition& to_write);
    std::ostream& operator<<(std::ostream& stream, const TypeInfo& to_write);
    std::ostream& operator<<(std::ostream& stream, const TypeInfoTable& to_write);
}
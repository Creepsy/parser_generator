#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <utility>

namespace type_parser {
    //identifier, type string
    typedef std::pair<std::string, std::string> Parameter;

    struct TypeDefinition {
        std::string identifier;
        
        std::vector<std::string> base_types;
        std::vector<Parameter> parameters;
    };

    std::ostream& operator<<(std::ostream& stream, const TypeDefinition& to_write);
}
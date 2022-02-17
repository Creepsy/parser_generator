#pragma once

#include <vector>
#include <string>
#include <cstddef>
#include <ostream>
#include <optional>

namespace rule_parser {
    struct RuleResult {
        bool create_new; // if false, expects arguments to have at least one member

        std::string type; 
        std::vector<std::optional<size_t>> arguments;
    };

    struct Parameter {
        bool is_token;

        std::string identifier;
    };

    struct RuleDefinition {
        bool is_entry;
        
        std::vector<Parameter> parameters;
        RuleResult result;
    };
    
    std::ostream& operator<<(std::ostream& stream, const RuleResult& to_write);
    std::ostream& operator<<(std::ostream& stream, const Parameter& to_write);
    std::ostream& operator<<(std::ostream& stream, const RuleDefinition& to_write);
}
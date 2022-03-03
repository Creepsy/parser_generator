#pragma once

#include <vector>
#include <string>
#include <cstddef>
#include <ostream>
#include <optional>

namespace rule_parser {
    struct RuleResult {
        enum ResultType {
            CREATE_NEW, // expects arguments to have at least one member (which is not of the type Token)
            PASS_ARG
        };

        ResultType result_type;
        std::string type; 
        std::vector<std::optional<size_t>> argument_ids;
    };

    struct Argument {
        bool is_token;
        bool is_vector;

        std::string identifier;
    };

    struct RuleDefinition {
        bool is_entry;
        
        std::vector<Argument> arguments;
        RuleResult result;
    };

    std::ostream& operator<<(std::ostream& stream, const RuleResult& to_write);
    std::ostream& operator<<(std::ostream& stream, const Argument& to_write);
    std::ostream& operator<<(std::ostream& stream, const RuleDefinition& to_write);

    bool operator==(const Argument& first, const Argument& second);

    //for std-containers
    bool operator<(const RuleResult& first, const RuleResult& second);
    bool operator<(const Argument& first, const Argument& second);
    bool operator<(const RuleDefinition& first, const RuleDefinition& second);
}
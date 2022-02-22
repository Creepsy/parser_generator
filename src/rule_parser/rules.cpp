#include "rules.h"

#include <utility>

#include "../util/comparators.h"

using namespace rule_parser;

std::ostream& rule_parser::operator<<(std::ostream& stream, const RuleResult& to_write) {
    if(to_write.result_type == RuleResult::PASS_ARG) 
        return stream << "$" << to_write.arguments.begin()->value_or(-1) << " -> " << to_write.type;

    stream << to_write.type << "(";

    for(int i = 0; i < to_write.arguments.size(); i++) {
        if(i != 0) stream << ", ";
        
        if(to_write.arguments[i].has_value()) {
            stream << "$" << to_write.arguments[i].value();
        } else {
            stream << "EMPTY";
        }
    }

    return stream << ") -> " << to_write.type;
}

std::ostream& rule_parser::operator<<(std::ostream& stream, const Parameter& to_write) {
    return stream << ((to_write.is_token) ? ("<" + to_write.identifier + ">") : to_write.identifier);
}

std::ostream& rule_parser::operator<<(std::ostream& stream, const RuleDefinition& to_write) {
    if(to_write.is_entry) stream << "* ";
    
    for(const Parameter& par : to_write.parameters) 
        stream << par << " ";

    return stream << ": " << to_write.result << ";";
}

bool rule_parser::operator<(const RuleResult& first, const RuleResult& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.result_type, second.result_type),
        std::make_pair(first.type, second.type),
        std::make_pair(first.arguments, second.arguments)
    );
}

bool rule_parser::operator<(const Parameter& first, const Parameter& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.is_token, second.is_token),
        std::make_pair(first.identifier, second.identifier)
    );
}

bool rule_parser::operator<(const RuleDefinition& first, const RuleDefinition& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.is_entry, second.is_entry),
        std::make_pair(first.parameters, second.parameters),
        std::make_pair(first.result, second.result)
    );
}
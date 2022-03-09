#include "rules.h"

#include <utility>

#include "../util/comparators.h"

using namespace rule_parser;

std::ostream& rule_parser::operator<<(std::ostream& stream, const RuleResult& to_write) {
    if(to_write.result_type == RuleResult::PASS_ARG) 
        return stream << "$" << to_write.argument_ids.begin()->value_or(-1) << " -> " << to_write.type;

    stream << to_write.type << "(";

    for(int i = 0; i < to_write.argument_ids.size(); i++) {
        if(i != 0) stream << ", ";
        
        if(to_write.argument_ids[i].has_value()) {
            stream << "$" << to_write.argument_ids[i].value();
        } else {
            stream << "EMPTY";
        }
    }

    return stream << ") -> " << to_write.type;
}

std::ostream& rule_parser::operator<<(std::ostream& stream, const Argument& to_write) {
    stream << ((to_write.is_token) ? ("<" + to_write.identifier + ">") : to_write.identifier);

    if(to_write.is_vector)
        stream << "[]";

    if(!to_write.scope.empty())
        stream << "#" << to_write.scope;

    return stream;
}

std::ostream& rule_parser::operator<<(std::ostream& stream, const RuleDefinition& to_write) {
    if(to_write.is_entry) stream << "* ";
    
    for(const Argument& arg : to_write.arguments) 
        stream << arg << " ";

    return stream << ": " << to_write.result << ";";
}

bool rule_parser::operator==(const Argument& first, const Argument& second) {
    return first.is_token == second.is_token && 
           first.is_vector == second.is_vector && 
           first.identifier == second.identifier &&
           first.scope == second.scope;
}

bool rule_parser::operator<(const RuleResult& first, const RuleResult& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.result_type, second.result_type),
        std::make_pair(first.type, second.type),
        std::make_pair(first.argument_ids, second.argument_ids)
    );
}

bool rule_parser::operator<(const Argument& first, const Argument& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.is_token, second.is_token),
        std::make_pair(first.is_vector, second.is_vector),
        std::make_pair(first.identifier, second.identifier),
        std::make_pair(first.scope, second.scope)
    );
}

bool rule_parser::operator<(const RuleDefinition& first, const RuleDefinition& second) {
    return comparators::smaller_lex_comparator(
        std::make_pair(first.is_entry, second.is_entry),
        std::make_pair(first.arguments, second.arguments),
        std::make_pair(first.result, second.result)
    );
}
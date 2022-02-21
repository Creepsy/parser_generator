#include "rules.h"

using namespace rule_parser;

std::ostream& rule_parser::operator<<(std::ostream& stream, const RuleResult& to_write) {
    if(to_write.result_type == RuleResult::PASS_ARG) return stream << "$" << to_write.arguments.begin()->value_or(-1) << " -> " << to_write.type;
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
    
    for(const Parameter& par : to_write.parameters) stream << par << " ";

    return stream << ": " << to_write.result << ";";
}
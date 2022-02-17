#include "rules.h"

using namespace rule_parser;

std::ostream& rule_parser::operator<<(std::ostream& stream, const RuleResult& to_write) {
    if(!to_write.create_new) return stream << "$" << to_write.arguments.begin()->value_or(-1);
    stream << to_write.type << "(";

    for(int i = 0; i < to_write.arguments.size(); i++) {
        if(i != 0) stream << ", ";
        stream << "$" << to_write.arguments[i].value_or(-1);
    }

    return stream << ")";
}

std::ostream& rule_parser::operator<<(std::ostream& stream, const Parameter& to_write) {
    return stream << ((to_write.is_token) ? ("<" + to_write.identifier + ">") : to_write.identifier);
}

std::ostream& rule_parser::operator<<(std::ostream& stream, const RuleDefinition& to_write) {
    if(to_write.is_entry) stream << "* ";
    
    for(const Parameter& par : to_write.parameters) stream << par << " ";

    return stream << ": " << to_write.result << ";";
}
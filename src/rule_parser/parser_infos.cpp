#include "parser_infos.h"

#include "../util.h"

using namespace parser_infos;

/*
rule_state::rule_state() {
}

rule_state::~rule_state() {
}
*/

std::ostream& parser_infos::operator<<(std::ostream& out, const type_parameter& to_print) {
    if(!to_print.identifer.empty()) out << to_print.identifer << ":";
    if(to_print.is_vec) {
        out << "vec[" << to_print.type_identifier << "]";
    } else {
        out << to_print.type_identifier;
    }

    for(const std::string& namsp : to_print.namespaces) out << "#" << namsp;

    return out;
}

std::ostream& parser_infos::operator<<(std::ostream& out, const value_parameter& to_print) {
    out << "[";
    return util::print_vector(out, to_print.members, ", ") << "]";
}

std::ostream& parser_infos::operator<<(std::ostream& out, const type_definition& to_print) {
    out << "def " << to_print.type_name;
    
    for(std::set<std::string>::const_iterator base_type = to_print.base_types.begin(); base_type != to_print.base_types.end(); base_type++) {
        if(base_type == to_print.base_types.begin()) {
            out << ": ";
        } else {
            out << ", ";
        }

        out << *base_type;
    }

    out << "(";
    return util::print_vector(out, to_print.members, ", ") << ");";
}

std::ostream& parser_infos::operator<<(std::ostream& out, const rule_result& to_print) {
    out << to_print.operand;

    if(to_print.type == rule_result::NEW_OBJECT) {
        out << "(";
        util::print_vector(out, to_print.args, ", ") << ")";
    }

    if(to_print.has_append) {
        out << " append (";
        util::print_vector(out, to_print.append_args, ", ") << ")";
    }

    return out;
}

std::ostream& parser_infos::operator<<(std::ostream& out, const rule& to_print) {
    if(!to_print.namesp.empty()) out << "#" << to_print.namesp << " ";
    if(to_print.is_entry_rule) out << "*";

    return util::print_vector(out, to_print.parameters, " ") << " -> " << to_print.result << ";";
}
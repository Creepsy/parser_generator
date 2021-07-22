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
        out << "append (";
        util::print_vector(out, to_print.append_args, ", ") << ")";
    }

    return out;
}

std::ostream& parser_infos::operator<<(std::ostream& out, const rule& to_print) {
    if(!to_print.group.empty()) out << to_print.group << " ";
    if(to_print.is_entry_rule) out << "*";

    return util::print_vector(out, to_print.parameters, " ") << " -> " << to_print.result << ";";
}


/*
bool parser_infos::operator<(const rule& first, const rule& second) {
    if(first.parameters != second.parameters) return first.parameters < second.parameters;
    if(second.result < first.result || first.result < second.result) return first.result < second.result;
    if(first.is_entry_rule != second.is_entry_rule) return first.is_entry_rule < second.is_entry_rule;

    return first.group < second.group;
}

bool parser_infos::operator<(const type_definition& first, const type_definition& second) {
    if(first.type_name != second.type_name) return first.type_name < second.type_name;
    if(first.members != second.members) return first.members < second.members;

    return first.base_types < second.base_types;
}

bool parser_infos::operator<(const rule_result& first, const rule_result& second) {
    if(first.type != second.type) return first.type < second.type;
    if(first.operand != second.operand) return first.operand < second.operand;
    if(first.args != second.args) return first.args < second.args;
    if(first.has_append != second.has_append) return first.has_append < second.has_append;

    return first.append_args < second.append_args;
}

bool parser_infos::operator<(const type_parameter& first, const type_parameter& second) {
    if(first.type != second.type) return first.type < second.type;
    if(first.type_identifier != second.type_identifier) return first.type_identifier < second.type_identifier;
    if(first.identifer != second.identifer) return first.identifer < second.identifer;
    if(first.is_vec != second.is_vec) return first.is_vec < second.is_vec;

    return first.namespaces < second.namespaces;
}

bool parser_infos::operator<(const value_parameter& first, const value_parameter& second) {
    return first.members < second.members;
}*/
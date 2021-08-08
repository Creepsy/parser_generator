#include "parser_infos.h"

#include "../util.h"

using namespace parser_infos;

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

std::ostream& parser_infos::operator<<(std::ostream& out, const value& to_print) {
    return out << to_print.identifier;
}

std::ostream& parser_infos::operator<<(std::ostream& out, const value_list& to_print) {
    out << "[";
    return util::print_iterator(out, to_print.members.begin(), to_print.members.end(), ", ") << "]";
}

std::ostream& parser_infos::operator<<(std::ostream& out, const type_definition& to_print) {
    out << "def " << to_print.type_name;
    
    if(!to_print.base_types.empty()) {
        out << ": ";
        util::print_iterator(out, to_print.base_types.begin(), to_print.base_types.end(), ", ");
    }

    out << "(";
    return util::print_iterator(out, to_print.members.begin(), to_print.members.end(), ", ") << ");";
}

std::ostream& parser_infos::operator<<(std::ostream& out, const rule_result& to_print) {
    out << to_print.operand;

    if(to_print.type == rule_result::NEW_OBJECT) {
        out << "(";
        util::print_iterator(out, to_print.args.begin(), to_print.args.end(), ", ") << ")";
    }

    if(to_print.has_append) {
        out << " append (";
        util::print_iterator(out, to_print.append_args.begin(), to_print.append_args.end(), ", ") << ")";
    }

    return out;
}

std::ostream& parser_infos::operator<<(std::ostream& out, const rule& to_print) {
    if(!to_print.namesp.empty()) out << "#" << to_print.namesp << " ";
    if(to_print.is_entry_rule) out << "*";

    return util::print_iterator(out, to_print.parameters.begin(), to_print.parameters.end(), " ") << " -> " << to_print.result << ";";
}
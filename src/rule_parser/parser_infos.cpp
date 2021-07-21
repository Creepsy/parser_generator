#include "parser_infos.h"

using namespace parser_infos;

rule_state::rule_state() {
}

rule_state::~rule_state() {
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
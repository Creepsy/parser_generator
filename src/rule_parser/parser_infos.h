#pragma once

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <utility>
#include <cstddef>

namespace parser_infos {
    struct type_parameter {
        enum parameter_type {
            TOKEN,
            SPECIFIC_TOKEN,
            CUSTOM_TYPE
        };

        parameter_type type;
        std::string type_identifier;

        std::string identifer = "";
        bool is_vec = false;
        std::set<std::string> namespaces = {};
    };

    struct value {
        enum value_type {
            NONE,
            PARAMETER
        };

        std::string identifier;
        value_type type;
    };

    struct value_list {
        std::vector<value> members;
    };

    struct type_definition {
        std::string type_name;
        std::vector<type_parameter> members;

        std::set<std::string> base_types = {};

        void complete_missing_names();
    };

    struct rule_result {
        enum result_type {
            NEW_OBJECT,
            PARAMETER
        };

        result_type type;
        std::string operand;

        std::vector<value_list> args = {};
        bool has_append = false;
        std::vector<value_list> append_args = {};
    };

    struct rule {
        std::vector<type_parameter> parameters;
        rule_result result;

        bool is_entry_rule = false;
        std::string namesp = "";

        std::set<std::pair<std::string, size_t>> get_parameters() const;
    };

    struct parser_info {
        std::vector<rule> entry_rules;
        std::vector<rule> parser_rules;
        std::vector<type_definition> types;

        std::set<std::string> get_base_type_names() const;
        std::set<std::string> get_type_names() const;
    };
    
    std::ostream& operator<<(std::ostream& out, const type_parameter& to_print);
    std::ostream& operator<<(std::ostream& out, const value& to_print);
    std::ostream& operator<<(std::ostream& out, const value_list& to_print);
    std::ostream& operator<<(std::ostream& out, const type_definition& to_print);
    std::ostream& operator<<(std::ostream& out, const rule_result& to_print);
    std::ostream& operator<<(std::ostream& out, const rule& to_print);
}
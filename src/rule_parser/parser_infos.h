#pragma once

#include <string>
#include <vector>
#include <set>
#include <iostream>

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

    struct value_parameter {
        std::vector<std::string> members;
    };

    struct type_definition {
        std::string type_name;
        std::vector<type_parameter> members;

        std::set<std::string> base_types = {};
    };

    struct rule_result {
        enum result_type {
            NEW_OBJECT,
            PARAMETER
        };

        result_type type;
        std::string operand;

        std::vector<value_parameter> args = {};
        bool has_append = false;
        std::vector<value_parameter> append_args = {};
    };

    struct rule {
        std::vector<type_parameter> parameters;
        rule_result result;

        bool is_entry_rule = false;
        std::string namesp = "";
    };

    struct parser_info {
        std::vector<rule> entry_rules;
        std::vector<rule> parser_rules;
        std::vector<type_definition> types;
    };

    /*class rule_state {
        private:
        public:
            rule_state();
            ~rule_state();
    };*/

    std::ostream& operator<<(std::ostream& out, const type_parameter& to_print);
    std::ostream& operator<<(std::ostream& out, const value_parameter& to_print);
    std::ostream& operator<<(std::ostream& out, const type_definition& to_print);
    std::ostream& operator<<(std::ostream& out, const rule_result& to_print);
    std::ostream& operator<<(std::ostream& out, const rule& to_print);
}
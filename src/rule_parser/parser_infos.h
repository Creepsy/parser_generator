#pragma once

#include <string>
#include <vector>
#include <set>

namespace parser_infos {
    struct type_parameter {
        enum parameter_type {
            TOKEN,
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
        std::string group = "";
    };

    struct parser_info {
        std::vector<rule> entry_rules;
        std::vector<rule> parser_rules;
        std::vector<type_definition> types;
       // std::set<std::string> base_types;
    };

    class rule_state {
        private:
        public:
            rule_state();
            ~rule_state();
    };

    /*bool operator<(const rule& first, const rule& second);
    bool operator<(const type_definition& first, const type_definition& second);
    bool operator<(const rule_result& first, const rule_result& second);
    bool operator<(const type_parameter& first, const type_parameter& second);
    bool operator<(const value_parameter& first, const value_parameter& second);*/
}
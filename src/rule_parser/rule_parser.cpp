#include "rule_parser.h"

#include <stdexcept>

using namespace rparser;

//helper functions
void throw_invalid_token_err(const token& found, const std::vector<token::token_type>& expected);
parser_infos::type_parameter::parameter_type parameter_type_from_token(const token& type_token);

void throw_invalid_token_err(const token& found, const std::vector<token::token_type>& expected) {
    std::string expected_str = "";
    for(size_t t = 0; t < expected.size(); t++) {
        if(t != 0) expected_str += ", ";
        expected_str += "'" + TOKEN_TYPE_TO_STR[expected[t]] + "'";
    }

    throw std::runtime_error("Invalid token at [l:" +
        std::to_string(found.pos.start_line) + ", c:" + std::to_string(found.pos.start_column) + 
        "]! Found '" + TOKEN_TYPE_TO_STR[found.type] + "', expected one of the following: {" + expected_str + "}!");
}

parser_infos::type_parameter::parameter_type parameter_type_from_token(const token& type_token) {
    switch (type_token.type) {
        case token::token_type::TOKEN:
            return parser_infos::type_parameter::SPECIFIC_TOKEN;
            break;
        case token::token_type::TOKEN_TYPE:
            return parser_infos::type_parameter::TOKEN;
            break;
        case token::token_type::IDENTIFIER:
            return parser_infos::type_parameter::CUSTOM_TYPE;
            break;
    }

    throw_invalid_token_err(type_token,
        std::vector<token::token_type>{
            token::token_type::IDENTIFIER,
            token::token_type::TOKEN,
            token::token_type::TOKEN_TYPE
        }
    );

    return parser_infos::type_parameter::TOKEN; //unreachable; make the compiler happy
}



rule_parser::rule_parser(rule_lexer& lexer) : lexer{lexer} {
    this->consume();
}

parser_infos::parser_info rule_parser::parse_parser_infos() {
    parser_infos::parser_info info{};

    while(this->curr.type != token::token_type::END_OF_FILE) {
        if(accept(token::token_type::DEFINE_TYPE)) {
            info.types.push_back(this->parse_type_definition());
        } else {
            parser_infos::rule parsed_rule = this->parse_rule();
            if(parsed_rule.is_entry_rule) {
                info.entry_rules.push_back(parsed_rule);
            } else {
                info.parser_rules.push_back(parsed_rule);
            }
        }
    }
    
    return info;
}

rule_parser::~rule_parser() {
}

//private
void rule_parser::expect(const token::token_type type) {
    if(!this->accept(type)) throw_invalid_token_err(this->curr, std::vector<token::token_type>{type});
}

void rule_parser::expect(const std::vector<token::token_type>& types) {
    if(!this->accept(types)) throw_invalid_token_err(this->curr, types);
}

bool rule_parser::accept(const token::token_type type) {
    return this->curr.type == type;
}

bool rule_parser::accept(const std::vector<token::token_type>& types) {
    for(const token::token_type type : types) {
        if(this->accept(type)) return true;
    }
    return false;
}

void rule_parser::consume() {
    this->curr = this->lexer.next_unignored_token();
}

void rule_parser::consume(const token::token_type type) {
    this->expect(type);
    this->consume();
}

parser_infos::type_definition rule_parser::parse_type_definition() {
    this->consume(token::token_type::DEFINE_TYPE);

    parser_infos::type_definition defined_type{};

    this->expect(token::token_type::IDENTIFIER);
    defined_type.type_name = this->curr.identifier;
    this->consume();

    if(this->accept(token::token_type::ASSIGN)) {
        do {
            this->consume();

            this->expect(token::token_type::IDENTIFIER);
            defined_type.base_types.insert(this->curr.identifier);
            this->consume();
        } while(this->accept(token::token_type::SEPERATOR));
    }

    this->consume(token::token_type::PAR_OPEN);
    while(!this->accept(token::token_type::PAR_CLOSE)) {
        if(!defined_type.members.empty()) this->consume(token::token_type::SEPERATOR);
        defined_type.members.push_back(this->parse_type_parameter());
    }
    this->consume(token::token_type::PAR_CLOSE);
    
    this->consume(token::token_type::EOL);

    return defined_type;
}

parser_infos::rule rule_parser::parse_rule() {
    return {};
}

parser_infos::type_parameter rule_parser::parse_type_parameter() {
    parser_infos::type_parameter type_param{};

    token type_token = this->curr;

    if(this->accept(token::token_type::IDENTIFIER)) {
        std::string identifier = this->curr.identifier;
        this->consume();

        if(this->accept(token::token_type::ASSIGN)) {
            this->consume();

            type_param.identifer = identifier;
            type_token = this->curr;
            this->consume();
        }
    } else {
        this->consume();
    }

    if(type_token.type == token::token_type::VECTOR) {
        type_param.is_vec = true;

        this->consume(token::token_type::SQ_BRACKET_OPEN);
        type_token = this->curr;
        this->consume();
        this->consume(token::token_type::SQ_BRACKET_CLOSE);
    }

    type_param.type_identifier = type_token.identifier;
    type_param.type = parameter_type_from_token(type_token);

    while(this->accept(token::token_type::NAMESPACE)) {
        type_param.namespaces.insert(this->curr.identifier.substr(1));
        this->consume();
    }

    return type_param;
}
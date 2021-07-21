#include "rule_parser.h"

#include <stdexcept>

using namespace rparser;

//helper functions
void throw_invalid_token_err(const token::token_type found, const std::vector<token::token_type>& expected);

void throw_invalid_token_err(const token::token_type found, const std::vector<token::token_type>& expected) {
    std::string expected_str = "";
    for(size_t t = 0; t < expected.size(); t++) {
        if(t != 0) expected_str += ", ";
        expected_str += "'" + TOKEN_TYPE_TO_STR[expected[t]] + "'";
    }

    throw std::runtime_error("Invalid token! Found '" + TOKEN_TYPE_TO_STR[found] + "', expected one of the following: {" + expected_str + "}!");
}



rule_parser::rule_parser(rule_lexer& lexer) : lexer{lexer} {
    this->consume();
}

parser_infos::parser_info rule_parser::parse_parser_infos() {
    return {};
}

rule_parser::~rule_parser() {
}

//private
void rule_parser::expect(const token::token_type type) {
    if(!this->accept(type)) throw_invalid_token_err(this->curr.type, std::vector<token::token_type>{type});
}

void rule_parser::expect(const std::vector<token::token_type>& types) {
    if(!this->accept(types)) throw_invalid_token_err(this->curr.type, types);
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
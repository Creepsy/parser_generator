#include "parser.h"

#include <string>
#include <stdexcept>
#include <cstddef>

using namespace type_parser;

TypeParser::TypeParser(type_lexer& lexer) : lexer(lexer) {
    this->consume();
}

TypeDefinition TypeParser::parse_next_type() {
    return {};
    //consume, expect, accept functions -> global for rule_parser?
    
}

bool TypeParser::end() {
    this->expect(token::token_type::ASSIGN);
    return this->lexer.end();
}

TypeParser::~TypeParser() {
}



//private

void TypeParser::expect(const token::token_type type) {
    if(!this->accept(type)) this->throw_parse_err();
}

bool TypeParser::accept(const token::token_type type) {
    return this->curr.type == type;
}

void TypeParser::consume() {
    this->curr = this->lexer.next_unignored_token();
}

void TypeParser::throw_parse_err() {
    constexpr auto format_position = [](size_t line, size_t column) -> std::string { return "[L:" + std::to_string(line) + ", C:" + std::to_string(column) + "]"; };

    throw std::runtime_error("Parsing Error at: \"" + this->curr.identifier + "\" " +
            format_position(this->curr.pos.start_line, this->curr.pos.start_column) + "-" + 
            format_position(this->curr.pos.end_line, this->curr.pos.end_column)
    );
}
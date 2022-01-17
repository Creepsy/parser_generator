#include "parser.h"

#include <string>
#include <stdexcept>
#include <cstddef>

using namespace type_parser;

TypeParser::TypeParser(type_lexer& lexer) : lexer(lexer) {
    this->consume();
}

TypeDefinition TypeParser::parse_next_type() {
    std::string type_identifier = this->consume(token::token_type::IDENTIFIER).identifier;

    this->consume(token::token_type::PAR_OPEN);

    //parse base types
    std::vector<std::string> base_types;
    
    while(!this->accept(token::token_type::PAR_CLOSE)) {
        if(!base_types.empty()) this->consume(token::token_type::SEPERATOR);

        base_types.push_back(this->consume(token::token_type::IDENTIFIER).identifier);
    }

    this->consume(token::token_type::PAR_CLOSE);
    this->consume(token::token_type::ASSIGN);

    //parse parameters
    std::vector<Parameter> parameters;

    while(!this->accept(token::token_type::EOL)) {
        if(!parameters.empty()) this->consume(token::token_type::SEPERATOR);

        std::string name = this->consume(token::token_type::IDENTIFIER).identifier;
        this->consume(token::token_type::ASSIGN);
        parameters.push_back(std::make_pair(name, this->consume(token::token_type::IDENTIFIER).identifier));
    }

    this->consume(token::token_type::EOL);

    return TypeDefinition{type_identifier, base_types, parameters};    
}

bool TypeParser::end() {
    return this->lexer.end();
}

TypeParser::~TypeParser() {
}



//private

void TypeParser::expect(const token::token_type to_expect) {
    if(!this->accept(to_expect)) this->throw_parse_err();
}

bool TypeParser::accept(const token::token_type to_check) {
    return this->curr.type == to_check;
}

token TypeParser::consume() {
    token consumed = this->curr;
    this->curr = this->lexer.next_unignored_token();

    return consumed;
}

token TypeParser::consume(const token::token_type to_expect) {
    this->expect(to_expect);

    return this->consume();
}

void TypeParser::throw_parse_err() {
    constexpr auto format_position = [](size_t line, size_t column) -> std::string { return "[L:" + std::to_string(line) + ", C:" + std::to_string(column) + "]"; };

    throw std::runtime_error("Parsing Error at: \"" + this->curr.identifier + "\" " +
            format_position(this->curr.pos.start_line, this->curr.pos.start_column) + "-" + 
            format_position(this->curr.pos.end_line, this->curr.pos.end_column)
    );
}
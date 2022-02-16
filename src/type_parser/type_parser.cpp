#include "type_parser.h"

#include <string>
#include <stdexcept>
#include <cstddef>

using namespace type_parser;

TypeParser::TypeParser(type_lexer::type_lexer& lexer) : ParserBase(lexer) {
}

TypeDefinition TypeParser::parse_next_type() {
    std::string type_identifier = this->consume(type_lexer::token::token_type::IDENTIFIER).identifier;

    this->consume(type_lexer::token::token_type::PAR_OPEN);

    //parse base types
    std::vector<std::string> base_types;
    
    while(!this->accept(type_lexer::token::token_type::PAR_CLOSE)) {
        if(!base_types.empty()) this->consume(type_lexer::token::token_type::SEPERATOR);

        base_types.push_back(this->consume(type_lexer::token::token_type::IDENTIFIER).identifier);
    }

    this->consume(type_lexer::token::token_type::PAR_CLOSE);
    this->consume(type_lexer::token::token_type::ASSIGN);

    //parse parameters
    std::vector<Parameter> parameters;

    while(!this->accept(type_lexer::token::token_type::EOL)) {
        if(!parameters.empty()) this->consume(type_lexer::token::token_type::SEPERATOR);

        std::string name = this->consume(type_lexer::token::token_type::IDENTIFIER).identifier;
        this->consume(type_lexer::token::token_type::ASSIGN);
        parameters.push_back(std::make_pair(name, this->consume(type_lexer::token::token_type::IDENTIFIER).identifier));
    }

    this->consume(type_lexer::token::token_type::EOL);

    return TypeDefinition{type_identifier, base_types, parameters};    
}

bool TypeParser::end() {
    return this->lexer.end();
}

TypeParser::~TypeParser() {
}
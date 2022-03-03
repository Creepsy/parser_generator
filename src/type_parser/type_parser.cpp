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
    std::set<std::string> base_types;
    
    while(!this->accept(type_lexer::token::token_type::PAR_CLOSE)) {
        if(!base_types.empty()) 
            this->consume(type_lexer::token::token_type::SEPERATOR);

        base_types.insert(this->consume(type_lexer::token::token_type::IDENTIFIER).identifier);
    }

    this->consume(type_lexer::token::token_type::PAR_CLOSE);
    this->consume(type_lexer::token::token_type::ASSIGN);

    //parse parameters
    std::vector<Parameter> parameters;

    while(!this->accept(type_lexer::token::token_type::EOL)) {
        if(!parameters.empty()) 
            this->consume(type_lexer::token::token_type::SEPERATOR);

        parameters.push_back(this->parser_parameter());
    }

    this->consume(type_lexer::token::token_type::EOL);

    return TypeDefinition{type_identifier, base_types, parameters};    
}

TypeParser::~TypeParser() {
}



//private
Parameter TypeParser::parser_parameter() {
    std::string name = this->consume(type_lexer::token::token_type::IDENTIFIER).identifier;
    this->consume(type_lexer::token::token_type::ASSIGN);
    std::string type = this->consume(type_lexer::token::token_type::IDENTIFIER).identifier;
    bool is_vector = false;

    if(this->accept(type_lexer::token::token_type::VECTOR)) {
        is_vector = true;
        this->consume();
    }
    
    return Parameter{name, type, is_vector};
}
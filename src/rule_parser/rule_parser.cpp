#include "rule_parser.h"

using namespace rule_parser;

rule_parser::RuleParser::RuleParser(rule_lexer::rule_lexer& lexer) : ParserBase(lexer) {
}

RuleDefinition rule_parser::RuleParser::parse_next_rule() {
    bool is_entry = false;
    if(this->accept(rule_lexer::token::ENTRY_POINT)) {
        this->consume();
        is_entry = true;
    }

    std::vector<Parameter> parameters = this->parse_parameters();

    this->consume(rule_lexer::token::ASSIGN);

    RuleResult result = this->parse_rule_result();

    this->consume(rule_lexer::token::EOL);

    return this->decorate_rule(RuleDefinition{is_entry, parameters, result});
}

rule_parser::RuleParser::~RuleParser() {
    
}



//private

std::vector<Parameter> rule_parser::RuleParser::parse_parameters() {
    std::vector<Parameter> parameters;
    do {
        if(this->accept(rule_lexer::token::TOKEN)) {
            std::string token_identifier = this->consume().identifier;
            parameters.push_back(Parameter{true, token_identifier.substr(1, token_identifier.size() - 2)});
        } else {
            parameters.push_back(Parameter{false, this->consume(rule_lexer::token::IDENTIFIER).identifier});
        }
    } while(!this->accept(rule_lexer::token::ASSIGN) && !this->end());

    return parameters;
}

RuleResult rule_parser::RuleParser::parse_rule_result() {
    if(this->accept(rule_lexer::token::INDEX)) {
        return RuleResult{RuleResult::PASS_ARG, "", std::vector<std::optional<size_t>>{std::stoull(this->consume().identifier.substr(1))}};
    }

    std::string type = this->consume(rule_lexer::token::IDENTIFIER).identifier;

    this->consume(rule_lexer::token::PAR_OPEN);

    std::vector<std::optional<size_t>> arguments;
    while(!this->accept(rule_lexer::token::PAR_CLOSE) && !this->end()) {
        if(!arguments.empty()) this->consume(rule_lexer::token::SEPERATOR);
        
        if(this->accept(rule_lexer::token::EMPTY)) {
            this->consume();
            arguments.push_back(std::nullopt);
        } else {
            arguments.push_back(std::stoull(this->consume(rule_lexer::token::INDEX).identifier.substr(1)));
        }
    }

    this->consume(rule_lexer::token::PAR_CLOSE);

    return RuleResult{RuleResult::CREATE_NEW, type, arguments};
}

RuleDefinition&& rule_parser::RuleParser::decorate_rule(RuleDefinition&& to_decorate) {
    if(to_decorate.result.result_type == RuleResult::PASS_ARG && !to_decorate.result.arguments.empty()) {
        size_t to_pass = to_decorate.result.arguments.begin()->value_or(-1);

        if(to_pass < to_decorate.parameters.size()) {
            if(to_decorate.parameters[to_pass].is_token) {
                to_decorate.result.type = "Token";
            } else {
                to_decorate.result.type = to_decorate.parameters[to_pass].identifier;
            }
        }
    }

    return std::move(to_decorate);
}
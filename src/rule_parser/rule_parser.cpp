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

    std::vector<Argument> args = this->parse_arguments();

    this->consume(rule_lexer::token::ASSIGN);

    RuleResult result = this->parse_rule_result();

    this->consume(rule_lexer::token::EOL);

    return this->decorate_rule(RuleDefinition{is_entry, args, result});
}

rule_parser::RuleParser::~RuleParser() {
    
}



//private

std::vector<Argument> rule_parser::RuleParser::parse_arguments() {
    std::vector<Argument> args;
    do {
        if(this->accept(rule_lexer::token::TOKEN)) {
            std::string token_identifier = this->consume().identifier;
            args.push_back(Argument{true, false, token_identifier.substr(1, token_identifier.size() - 2)});
        } else {
            std::string identifier = this->consume(rule_lexer::token::IDENTIFIER).identifier;

            bool is_vector = false;
            if(this->accept(rule_lexer::token::VECTOR)) {
                is_vector = true;
                this->consume();
            }

            std::string scope;
            if(this->accept(rule_lexer::token::SCOPE))
                scope = this->consume().identifier.substr(1);

            args.push_back(Argument{false, is_vector, identifier, scope});
        }
    } while(!this->accept(rule_lexer::token::ASSIGN) && !this->end());

    return args;
}

RuleResult rule_parser::RuleParser::parse_rule_result() {
    if(this->accept(rule_lexer::token::INDEX)) {
        size_t index = std::stoull(this->consume().identifier.substr(1));

        std::string scope;
        if(this->accept(rule_lexer::token::SCOPE))
            scope = this->consume().identifier.substr(1);

        return RuleResult{RuleResult::PASS_ARG, Argument{false, false, "", scope}, std::vector<std::optional<size_t>>{index}};
    }

    std::string type = this->consume(rule_lexer::token::IDENTIFIER).identifier;
    bool is_vector = false;

    if(this->accept(rule_lexer::token::VECTOR)) {
        is_vector = true;
        this->consume();
    }

    this->consume(rule_lexer::token::PAR_OPEN);

    std::vector<std::optional<size_t>> arguments = this->parse_result_arguments(rule_lexer::token::PAR_CLOSE);    

    this->consume(rule_lexer::token::PAR_CLOSE);

    std::string scope;
    if(this->accept(rule_lexer::token::SCOPE))
        scope = this->consume().identifier.substr(1);

    return RuleResult{(is_vector) ? RuleResult::CREATE_VECTOR : RuleResult::CREATE_NEW, Argument{false, is_vector, type, scope}, arguments};
}

std::vector<std::optional<size_t>> rule_parser::RuleParser::parse_result_arguments(const rule_lexer::token::token_type terminating_token) {
    std::vector<std::optional<size_t>> arguments;
    while(!this->accept(terminating_token) && !this->end()) {
        if(!arguments.empty()) this->consume(rule_lexer::token::SEPERATOR);
        
        if(this->accept(rule_lexer::token::EMPTY)) {
            this->consume();
            arguments.push_back(std::nullopt);
        } else {
            arguments.push_back(std::stoull(this->consume(rule_lexer::token::INDEX).identifier.substr(1)));
        }
    }

    return arguments;
}

RuleDefinition&& rule_parser::RuleParser::decorate_rule(RuleDefinition&& to_decorate) {
    if(to_decorate.result.result_type == RuleResult::PASS_ARG && !to_decorate.result.argument_ids.empty()) {
        size_t to_pass = to_decorate.result.argument_ids.begin()->value_or(-1);

        if(to_pass < to_decorate.arguments.size()) {
            std::string scope = to_decorate.result.type.scope;
            
            if(to_decorate.arguments[to_pass].is_token) {
                to_decorate.result.type = Argument{true, false, "Token"};
            } else {
                to_decorate.result.type = to_decorate.arguments[to_pass];
            }

            to_decorate.result.type.scope = scope;
        }
    }

    if(to_decorate.result.type.identifier == "Token")
        to_decorate.result.type.is_token = true;

    return std::move(to_decorate);
}
#pragma once

template <typename token_t>
concept TokenBase = requires (token_t t){
    typename token_t::token_type;
    t.pos;
    t.pos.start_line;
    t.pos.start_column;
    t.pos.end_line;
    t.pos.end_column;
    t.type;
    t.identifier;
};

template<typename lexer_t, TokenBase token_t>
class ParserBase {
    protected:
        lexer_t lexer;
        token_t curr;

        void expect(const token_t::token_type to_expect);
        bool accept(const token_t::token_type to_check);
        token_t consume();
        token_t consume(const token_t::token_type to_expect);

        void throw_parse_err();
    public:
        ParserBase(lexer_t& lexer);
        virtual ~ParserBase();
};

#include "parser_base.ipp"
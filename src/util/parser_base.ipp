template<typename lexer_t, TokenBase token_t>
ParserBase<lexer_t, token_t>::ParserBase(lexer_t& lexer) : lexer(lexer) {
    this->consume();
}

template<typename lexer_t, TokenBase token_t>
bool ParserBase<lexer_t, token_t>::end() {
    return this->curr.type == token_t::token_type::END_OF_FILE;
}

template<typename lexer_t, TokenBase token_t>
ParserBase<lexer_t, token_t>::~ParserBase() {
}



//protected

template<typename lexer_t, TokenBase token_t>
void ParserBase<lexer_t, token_t>::expect(const token_t::token_type to_expect) {
    if(!this->accept(to_expect)) this->throw_parse_err();
}

template <typename lexer_t, TokenBase token_t>
bool ParserBase<lexer_t, token_t>::accept(const token_t::token_type to_check) {
    return this->curr.type == to_check;
}

template <typename lexer_t, TokenBase token_t>
token_t ParserBase<lexer_t, token_t>::consume() {
    token_t consumed = this->curr;
    this->curr = this->lexer.next_unignored_token();

    return consumed;
}

template <typename lexer_t, TokenBase token_t>
token_t ParserBase<lexer_t, token_t>::consume(const token_t::token_type to_expect) {
    this->expect(to_expect);

    return this->consume();
}

template <typename lexer_t, TokenBase token_t>
void ParserBase<lexer_t, token_t>::throw_parse_err() {
    constexpr auto format_position = [](size_t line, size_t column) -> std::string { return "[L:" + std::to_string(line) + ", C:" + std::to_string(column) + "]"; };

    throw std::runtime_error("Parsing Error at: \"" + this->curr.identifier + "\" " +
            format_position(this->curr.pos.start_line, this->curr.pos.start_column) + "-" + 
            format_position(this->curr.pos.end_line, this->curr.pos.end_column)
    );
}
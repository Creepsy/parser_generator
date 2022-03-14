#include "lexer.h"

#include <set>
using namespace lexer;

const std::set<token::token_type> IGNORED_TOKENS {
	token::WSPACE
};

lexer::lexer::lexer::lexer(std::istream& stream) : stream(stream), curr_line(0), curr_column(0), curr('\0') {
	this->next();
	this->curr_column--;
}

bool lexer::lexer::end() {
	return this->stream.fail();
}

token lexer::lexer::next_token() {
	size_t start_line = this->curr_line;
	size_t start_column = this->curr_column;
	if(this->end()) return token{"EOF", token::END_OF_FILE, position{start_line, start_column, start_line, start_column}};

	std::string identifier = "";
	size_t state = 0;

	while(true) {
		switch(state) {
			case 0:
				switch(this->curr) {
					case 9 ... 13:
					case 32:
						state = 1;
						break;
					case 48 ... 57:
						state = 2;
						break;
					case 43:
						state = 3;
						break;
					case 44:
						state = 4;
						break;
					case 110:
						state = 5;
						break;
					default:
						state = 18446744073709551615ULL;
						break;
				}
				break;
			case 1:
				switch(this->curr) {
					case 9 ... 13:
					case 32:
						state = 1;
						break;
					default:
						return token{identifier, token::WSPACE, position{start_line, start_column, this->curr_line, this->curr_column}};
				}
				break;
			case 2:
				switch(this->curr) {
					case 48 ... 57:
						state = 2;
						break;
					default:
						return token{identifier, token::INT, position{start_line, start_column, this->curr_line, this->curr_column}};
				}
				break;
			case 3:
				switch(this->curr) {
					default:
						return token{identifier, token::ADD, position{start_line, start_column, this->curr_line, this->curr_column}};
				}
				break;
			case 4:
				switch(this->curr) {
					default:
						return token{identifier, token::COMMA, position{start_line, start_column, this->curr_line, this->curr_column}};
				}
				break;
			case 5:
				switch(this->curr) {
					case 117:
						state = 6;
						break;
					default:
						state = 18446744073709551615ULL;
						break;
				}
				break;
			case 6:
				switch(this->curr) {
					case 109:
						state = 7;
						break;
					default:
						state = 18446744073709551615ULL;
						break;
				}
				break;
			case 7:
				switch(this->curr) {
					default:
						return token{identifier, token::NUM, position{start_line, start_column, this->curr_line, this->curr_column}};
				}
				break;
			default:
				return token{identifier, token::UNDEFINED, position{start_line, start_column, this->curr_line, this->curr_column}};
		}

		identifier += this->curr;
		this->next();
	}
	return token{"", token::UNDEFINED, position{}};
}

token lexer::lexer::next_unignored_token() {
	token t = this->next_token();
	while(t.type != token::UNDEFINED && t.type != token::END_OF_FILE && IGNORED_TOKENS.find(t.type) != IGNORED_TOKENS.end()) {
		t = this->next_token();
	}
	return t;
}

lexer::lexer::~lexer() {
}

//private

void lexer::lexer::next() {
	this->curr_column++;
	if(this->curr == '\n') {
		this->curr_column = 0;
		this->curr_line++;
	}

	this->curr = this->stream.get();
}

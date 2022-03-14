#include "parser.h"

#include <utility>
#include <algorithm>
#include <vector>
#include <iterator>
#include <stdexcept>

using namespace parser;

Parser::Parser(lexer::lexer lex) : lex(lex) {
	this->curr = this->lex.next_unignored_token();
}

ParseStackElement Parser::parse_next() {
	while(true) {
		switch(this->get_state()) {
			case 8:
				switch(this->curr.type) {
					case lexer::token::INT:
						this->shift(1);
						break;
						break;
						break;
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 3:
				switch(this->curr.type) {
					case lexer::token::ADD:
						this->shift(2);
						break;
					case lexer::token::COMMA:
					case lexer::token::END_OF_FILE:
						this->reduce_3();
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 5:
				switch(this->curr.type) {
					case lexer::token::INT:
						this->shift(1);
						break;
						break;
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 4:
				switch(this->curr.type) {
					case lexer::token::ADD:
						this->shift(2);
						break;
					case lexer::token::COMMA:
					case lexer::token::END_OF_FILE:
						this->reduce_4();
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 2:
				switch(this->curr.type) {
					case lexer::token::INT:
						this->shift(1);
						break;
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 0:
				switch(this->curr.type) {
					case lexer::token::ADD:
					case lexer::token::COMMA:
					case lexer::token::END_OF_FILE:
						this->reduce_1();
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 7:
				switch(this->curr.type) {
					case lexer::token::COMMA:
						this->shift(5);
						break;
					case lexer::token::END_OF_FILE:
						this->shift(6);
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 1:
				switch(this->curr.type) {
					case lexer::token::ADD:
					case lexer::token::COMMA:
					case lexer::token::END_OF_FILE:
						this->reduce_2();
						break;
					default:
						this->throw_parse_error();
						break;
				}
				break;
			case 6:
				switch(this->curr.type) {
					default:
						return this->reduce_0();
						break;
				}
				break;
			default:
				this->throw_parse_error();
		break;
		}
	}
}

bool Parser::end() {
	return this->curr.type == lexer::token::END_OF_FILE;
}

Parser::~Parser() {
}



void Parser::shift(const size_t state_id) {
	this->parse_stack.push(ParseStackElement{this->curr, ElementType::TOKEN, state_id});
	this->curr = this->lex.next_unignored_token();
}

void Parser::goto_table(ParseStackElement&& parsed) {
	switch(this->get_state()) {
		case 8:
			switch(parsed.type) {
				case ElementType::ADDITION:
					parsed.state_id = 3;
					this->parse_stack.push(std::move(parsed));
					break;
				case ElementType::INTEGER:
					parsed.state_id = 3;
					this->parse_stack.push(std::move(parsed));
					break;
				case ElementType::VEC_NUMERIC:
					parsed.state_id = 7;
					this->parse_stack.push(std::move(parsed));
					break;
				default:
					this->throw_parse_error();
			}
			break;
		case 3:
			switch(parsed.type) {
				default:
					this->throw_parse_error();
			}
			break;
		case 5:
			switch(parsed.type) {
				case ElementType::ADDITION:
					parsed.state_id = 4;
					this->parse_stack.push(std::move(parsed));
					break;
				case ElementType::INTEGER:
					parsed.state_id = 4;
					this->parse_stack.push(std::move(parsed));
					break;
				default:
					this->throw_parse_error();
			}
			break;
		case 4:
			switch(parsed.type) {
				default:
					this->throw_parse_error();
			}
			break;
		case 2:
			switch(parsed.type) {
				case ElementType::INTEGER:
					parsed.state_id = 0;
					this->parse_stack.push(std::move(parsed));
					break;
				default:
					this->throw_parse_error();
			}
			break;
		case 0:
			switch(parsed.type) {
				default:
					this->throw_parse_error();
			}
			break;
		case 7:
			switch(parsed.type) {
				default:
					this->throw_parse_error();
			}
			break;
		case 1:
			switch(parsed.type) {
				default:
					this->throw_parse_error();
			}
			break;
		case 6:
			switch(parsed.type) {
				default:
					this->throw_parse_error();
			}
			break;
		default:
			this->throw_parse_error();
	}
}

void Parser::throw_parse_error() {
	constexpr auto format_position = [](size_t line, size_t column) -> std::string { return "[L:" + std::to_string(line) + ", C:" + std::to_string(column) + "]"; };
	throw std::runtime_error("Error while parsing " + this->curr.identifier + " at " + format_position(this->curr.pos.start_line, this->curr.pos.start_column) +  "! ");
}

size_t Parser::get_state() {
	return (this->parse_stack.empty()) ? 8 : this->parse_stack.top().state_id;
}

void Parser::reduce_3() {
	std::shared_ptr<TypeBase> arg_0 = std::any_cast<std::shared_ptr<TypeBase>>(this->parse_stack.top().element); this->parse_stack.pop();

	std::vector<std::shared_ptr<TypeBase>> collected;

	collected.push_back(arg_0);

	this->goto_table(std::move(ParseStackElement{std::move(collected), ElementType::VEC_NUMERIC, 0}));
}

void Parser::reduce_1() {
	std::shared_ptr<TypeBase> arg_2 = std::any_cast<std::shared_ptr<TypeBase>>(this->parse_stack.top().element); this->parse_stack.pop();
	lexer::token arg_1 = std::any_cast<lexer::token>(this->parse_stack.top().element); this->parse_stack.pop();
	std::shared_ptr<TypeBase> arg_0 = std::any_cast<std::shared_ptr<TypeBase>>(this->parse_stack.top().element); this->parse_stack.pop();

	std::shared_ptr<Numeric> conv_0 = std::dynamic_pointer_cast<Numeric>(arg_0);
	std::shared_ptr<Numeric> conv_2 = std::dynamic_pointer_cast<Numeric>(arg_2);
	ParseStackElement result{std::dynamic_pointer_cast<TypeBase>(std::make_shared<Addition>(std::move(conv_0), std::move(arg_1), std::move(conv_2))), ElementType::ADDITION, 0};
	this->goto_table(std::move(result));
}

void Parser::reduce_5() {
	std::vector<std::shared_ptr<TypeBase>> arg_0 = std::any_cast<std::vector<std::shared_ptr<TypeBase>>>(this->parse_stack.top().element); this->parse_stack.pop();

	std::vector<std::shared_ptr<Value>> conv_0;
	std::transform(arg_0.begin(), arg_0.end(), std::back_inserter(conv_0), 
		[](const std::shared_ptr<TypeBase>& ptr) -> std::shared_ptr<Value> {
			return std::dynamic_pointer_cast<Value>(ptr);
		}
	);
	ParseStackElement result{std::dynamic_pointer_cast<TypeBase>(std::make_shared<List>(std::move(conv_0))), ElementType::LIST, 0};
	this->goto_table(std::move(result));
}

void Parser::reduce_4() {
	std::shared_ptr<TypeBase> arg_2 = std::any_cast<std::shared_ptr<TypeBase>>(this->parse_stack.top().element); this->parse_stack.pop();
	lexer::token arg_1 = std::any_cast<lexer::token>(this->parse_stack.top().element); this->parse_stack.pop();
	std::vector<std::shared_ptr<TypeBase>> arg_0 = std::any_cast<std::vector<std::shared_ptr<TypeBase>>>(this->parse_stack.top().element); this->parse_stack.pop();

	std::vector<std::shared_ptr<TypeBase>> collected;

	std::copy(arg_0.begin(), arg_0.end(), std::back_inserter(collected));
	collected.push_back(arg_2);

	this->goto_table(std::move(ParseStackElement{std::move(collected), ElementType::VEC_NUMERIC, 0}));
}

void Parser::reduce_2() {
	lexer::token arg_0 = std::any_cast<lexer::token>(this->parse_stack.top().element); this->parse_stack.pop();

	ParseStackElement result{std::dynamic_pointer_cast<TypeBase>(std::make_shared<Integer>(std::move(arg_0))), ElementType::INTEGER, 0};
	this->goto_table(std::move(result));
}

ParseStackElement Parser::reduce_0() {
	lexer::token arg_1 = std::any_cast<lexer::token>(this->parse_stack.top().element); this->parse_stack.pop();
	std::vector<std::shared_ptr<TypeBase>> arg_0 = std::any_cast<std::vector<std::shared_ptr<TypeBase>>>(this->parse_stack.top().element); this->parse_stack.pop();

	std::vector<std::shared_ptr<Numeric>> conv_0;
	std::transform(arg_0.begin(), arg_0.end(), std::back_inserter(conv_0), 
		[](const std::shared_ptr<TypeBase>& ptr) -> std::shared_ptr<Numeric> {
			return std::dynamic_pointer_cast<Numeric>(ptr);
		}
	);
	return ParseStackElement{conv_0, ElementType::VEC_NUMERIC, 0};
}


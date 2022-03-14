#pragma once

#include <any>
#include <cstddef>
#include <stack>

#include "lexer.h"
#include "types.h"

namespace parser {
	enum ElementType {
		ADDITION,
		INTEGER,
		LIST,
		NUMERIC,
		TOKEN,
		VEC_NUMERIC
	};

	struct ParseStackElement {
		std::any element;
		ElementType type;
		size_t state_id;
	};

	class Parser {
		private:
			lexer::lexer lex;
			lexer::token curr;
			std::stack<ParseStackElement> parse_stack;

			void shift(const size_t state_id);
			void goto_table(ParseStackElement&& parsed);
			void throw_parse_error();
			size_t get_state();

			void reduce_3();
			void reduce_1();
			void reduce_5();
			void reduce_4();
			void reduce_2();
			ParseStackElement reduce_0();
		public:
			Parser(lexer::lexer lex);
			ParseStackElement parse_next();
			bool end();
			~Parser();
	};

}

#pragma once

#include <cstddef>
#include <string>
#include <istream>

namespace lexer {
	struct position {
		size_t start_line;
		size_t start_column;
		size_t end_line;
		size_t end_column;
	};
	
	struct token {
		enum token_type {
			END_OF_FILE,
			UNDEFINED,
			WSPACE,
			INT,
			ADD,
			COMMA,
			NUM
		};

		std::string identifier;
		token_type type;
		position pos;
	};

	class lexer {
		private:
			std::istream& stream;

			size_t curr_line;
			size_t curr_column;

			char curr;

			void next();
	   public:
			lexer(std::istream& stream);
			bool end();
			token next_token();
			token next_unignored_token();
			~lexer();
	};
}

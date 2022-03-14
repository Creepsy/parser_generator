#include <iostream>
#include <sstream>

#include "parser.h"

int main() {
    std::stringstream input{"6 + 100, 7 + 4 + 3, 6 + 25932"};

    lexer::lexer lex{input};
    parser::Parser pars{lex};

    parser::ParseStackElement out = pars.parse_next();
    if(out.type != parser::ElementType::VEC_NUMERIC) {
        std::cerr << "Unexpected parse result!" << std::endl;
        return -1;
    }

    std::vector<std::shared_ptr<parser::Numeric>> list = std::any_cast<std::vector<std::shared_ptr<parser::Numeric>>>(out.element);

    for(const std::shared_ptr<parser::Numeric>& to_calculate : list) {
        std::cout << to_calculate->calc() << " ";
    }
    std::cout << std::endl;

    return 0;
}
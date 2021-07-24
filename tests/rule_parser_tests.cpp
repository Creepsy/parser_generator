#include "rule_parser_tests.h"

#include <sstream>

#include <rule_parser/rule_parser.h>
#include <rule_parser/parser_infos.h>
#include <rule_lexer.h>

#include "test_util.h"

using namespace rparser_tests;

void rparser_tests::test() {
    test_rule_parsing();
    test_input_parsing();
    test_type_parsing();
}

void rparser_tests::test_rule_parsing() {

}

void rparser_tests::test_type_parsing() {
    std::stringstream valid_type_str {
        "def  atype   \t\t: value,   number, type(members:vec[value],    another:Token,  v  :number); //a type definition\n"
    };

    rule_lexer valid_lex{valid_type_str};
    rparser::rule_parser valid_pars{valid_lex};

    std::stringstream parser_result{""};
    parser_result << valid_pars.parse_type_definition();
    std::cout << parser_result.str() << std::endl;
    tutils::test_for_success(parser_result.str() == "def atype: number, type, value(members:vec[value], another:Token, v:number);", "valid type parsing");
    tutils::test_for_success(valid_pars.end(), "reached EOF");
    
    //TODO: test for error
}

void rparser_tests::test_input_parsing() {

}
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "rule_lexer.h"
#include "rule_parser/rule_parser.h"

int main() {
    std::cout << "project setup!" << std::endl;

    std::stringstream input{
        "//Types\n"
        "def list:value(members:vec[value]); // A list type\n"
        "def int:value, numeric(num:Token);\n"
        "def addition:value, numeric(first:numeric, operator:Token, second:numeric);\n"
        "\n"
        "//Rules\n"
        "*l:list <EOF> -> l;\n"
        "<PARO> l:list#enumeration <PARC> -> l;\n"
        "n:<INT> -> int(n);\n"
        "f:numeric op:<ADD> s:int -> addition(f, op, s);\n"
        "#enumeration v:value -> list([v]);\n"
        "#enumeration l:list#enumeration <SEP> v:value -> l append ([v]);\n"
        "test:none_append -> test append (?, []);"
    };

    rule_lexer lex{input};

    while(true) {
        token t = lex.next_unignored_token();
        std::cout << rparser::TOKEN_TYPE_TO_STR[t.type] << " -> " << t.identifier
                  << ", [" << t.pos.start_line << "," << t.pos.start_column << "] to [" << t.pos.end_line << "," << t.pos.end_column << "]" << std::endl;
        if(t.type == token::END_OF_FILE) break;
    }

    return 0;
}
cmake_minimum_required(VERSION 3.10)
project(testing)

set(TESTING_HEADERS
    "lexer.h"
    "types.h"
    "parser.h"    
)

set(TESTING_SOURCES
    "lexer.cpp"
    "parser.cpp"
)

add_executable(test testing.cpp ${TESTING_HEADERS} ${TESTING_SOURCES})
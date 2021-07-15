cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(parser_generator)

add_subdirectory(lexer_generator)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/rule_lexer.cpp ${CMAKE_BINARY_DIR}/rule_lexer.h
    COMMAND lexer_generator ${CMAKE_CURRENT_SOURCE_DIR}/src/rule_parser/lexer_rules.txt ${CMAKE_BINARY_DIR}/ rule_lexer
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/rule_parser/lexer_rules.txt
)

set(PROJECT_HEADERS
    ${CMAKE_BINARY_DIR}/rule_lexer.h
)

set(PROJECT_SOURCE_FILES
    ${CMAKE_BINARY_DIR}/rule_lexer.cpp
)

add_executable(parser_generator src/generator.cpp ${PROJECT_HEADERS} ${PROJECT_SOURCE_FILES})
target_include_directories(parser_generator PRIVATE ${CMAKE_BINARY_DIR})
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/rule_lexer.cpp ${CMAKE_BINARY_DIR}/rule_lexer.h
    COMMAND lexer_generator ${CMAKE_CURRENT_SOURCE_DIR}/lexer_rules.txt ${CMAKE_BINARY_DIR}/ rule_lexer
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer_rules.txt
)

file(GLOB_RECURSE RULE_PARSER_HEADERS
    "*.h"
)

file(GLOB_RECURSE RULE_PARSER_SOURCES
    "*.cpp"
)

file(GLOB_RECURSE RULE_PARSER_INLINE_FILES
    "*.ipp"
)

set(LEXER_FILES
    ${CMAKE_BINARY_DIR}/rule_lexer.cpp
    ${CMAKE_BINARY_DIR}/rule_lexer.h
)

add_library(rule_parser_lib ${PROJECT_HEADERS} ${PROJECT_SOURCE_FILES} ${PROJECT_INLINE_FILES} ${LEXER_FILES})
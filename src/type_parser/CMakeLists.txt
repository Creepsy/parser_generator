add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/type_lexer.cpp ${CMAKE_BINARY_DIR}/type_lexer.h
    COMMAND lexer_generator ${CMAKE_CURRENT_SOURCE_DIR}/lexer_rules.txt ${CMAKE_BINARY_DIR}/ type_lexer
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lexer_rules.txt
)

file(GLOB_RECURSE TYPE_PARSER_HEADERS
    "*.h"
)

file(GLOB_RECURSE TYPE_PARSER_SOURCES
    "*.cpp"
)

file(GLOB_RECURSE TYPE_PARSER_INLINE_FILES
    "*.ipp"
)

set(LEXER_FILES
    ${CMAKE_BINARY_DIR}/type_lexer.cpp
    ${CMAKE_BINARY_DIR}/type_lexer.h
)

add_library(type_parser_lib ${TYPE_PARSER_HEADERS} ${TYPE_PARSER_SOURCES} ${TYPE_PARSER_INLINE_FILES} ${LEXER_FILES})
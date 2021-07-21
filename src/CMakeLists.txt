add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/rule_lexer.cpp ${CMAKE_BINARY_DIR}/rule_lexer.h
    COMMAND lexer_generator ${CMAKE_CURRENT_SOURCE_DIR}/rule_parser/lexer_rules.txt ${CMAKE_BINARY_DIR}/ rule_lexer
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rule_parser/lexer_rules.txt
)

set(PROJECT_HEADERS
    ${CMAKE_BINARY_DIR}/rule_lexer.h
    rule_parser/parser_infos.h
    rule_parser/rule_parser.h
)

set(PROJECT_SOURCE_FILES
    ${CMAKE_BINARY_DIR}/rule_lexer.cpp
    rule_parser/parser_infos.cpp
    rule_parser/rule_parser.cpp
)

add_library(parser_generator_src ${PROJECT_HEADERS} ${PROJECT_SOURCE_FILES})

include_directories(${CMAKE_BINARY_DIR})
add_executable(parser_generator generator.cpp)
target_link_libraries(parser_generator parser_generator_src)
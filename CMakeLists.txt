cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

project(parser_generator)

add_subdirectory(lexer_generator)
add_subdirectory(src)

add_subdirectory(tests)
enable_testing()
add_test(NAME parser_generator_test COMMAND tests/parser_generator_tests)
set_tests_properties(parser_generator_test PROPERTIES FAIL_REGULAR_EXPRESSION ".*FAILED.*")
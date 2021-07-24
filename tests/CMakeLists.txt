include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)

set(TEST_SOURCE_FILES
    test_util.cpp
    rule_parser_tests.cpp
)

set(TEST_HEADERS
    test_util.h
    rule_parser_tests.h
)

add_executable(parser_generator_tests tests.cpp ${TEST_SOURCE_FILES} ${TEST_HEADERS})
target_link_libraries(parser_generator_tests parser_generator_src)
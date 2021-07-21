include_directories(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src)

add_executable(parser_generator_tests tests.cpp)
target_link_libraries(parser_generator_tests parser_generator_src)
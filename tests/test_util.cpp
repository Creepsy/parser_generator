#include "test_util.h"

#include <iostream>

using namespace tutils;

//helper functions
void print_test_result(const std::string& test_info, const bool passed);

void print_test_result(const std::string& test_info, const bool passed) {
    std::cout << "Test '" << test_info << "': ";

    if(passed) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cout << "FAILED" << std::endl;
    }
}



void tutils::test_for_success(const bool result, const std::string& test_info) {
    print_test_result(test_info, result);
}

void tutils::test_for_error(const test_func to_test, const std::string& test_info) {
    try {
        to_test();

        print_test_result(test_info, false);
    } catch(...) {
        print_test_result(test_info, true);
    }
}
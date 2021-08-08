#pragma once

#include <string>

namespace tutils {
    typedef void (*test_func)();

    void test_for_success(const bool result, const std::string& test_info);
    void test_for_error(const test_func to_test, const std::string& test_info);
}
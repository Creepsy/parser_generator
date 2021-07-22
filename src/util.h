#pragma once

#include <iostream>
#include <vector>

namespace util {
    template<class t>
    std::ostream& print_vector(std::ostream& out, const std::vector<t> to_print, const std::string& seperator);
};

#include "util.ipp"
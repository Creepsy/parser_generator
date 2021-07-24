#pragma once

#include <iostream>
#include <vector>

namespace util {
    template<class iterator>
    std::ostream& print_iterator(std::ostream& out, const iterator& start, const iterator& end, const std::string& seperator);
};

#include "util.ipp"
#pragma once

#include <iostream>
#include <vector>
#include <string>

namespace util {
    const std::string BASE_NODE_NAME = "node_base";
    const std::string TOKEN_NODE_NAME = "token_node";

    template<class iterator>
    std::ostream& print_iterator(std::ostream& out, const iterator& start, const iterator& end, const std::string& seperator);
};

#include "util.ipp"
#pragma once

namespace comparators {
    template<typename... args_t>
    bool smaller_lex_comparator(std::pair<args_t, args_t>... args);
}

#include "comparators.ipp"
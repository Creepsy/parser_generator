template<typename first_t, typename... args_t>
inline bool comparators::smaller_lex_comparator(std::pair<first_t, first_t> head, args_t... remaining) {
    if(head.first < head.second) return true;

    return (head.second < head.first) ? false : smaller_lex_comparator(remaining...);
}

template<>
inline bool comparators::smaller_lex_comparator() {
    return false;
}
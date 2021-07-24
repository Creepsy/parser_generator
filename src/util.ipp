template<class iterator>
std::ostream& util::print_iterator(std::ostream& out, const iterator& start, const iterator& end, const std::string& seperator) {
    for(iterator curr = start; curr != end; curr++) {
        if(curr != start) out << seperator;
        out << *curr;
    }

    return out;
}
template<class t>
std::ostream& util::print_vector(std::ostream& out, const std::vector<t> to_print, const std::string& seperator) {
    for(size_t element = 0; element < to_print.size(); element++) {
        if(element != 0) out << seperator;
        out << to_print[element];
    }

    return out;
}
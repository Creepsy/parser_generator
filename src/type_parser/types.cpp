#include "types.h"

using namespace type_parser;

std::ostream& type_parser::operator<<(std::ostream& stream, const TypeDefinition& to_write) {
    stream << to_write.identifier << "( ";
    for(const std::string& base : to_write.base_types) stream << base << " ";
    stream << ") {\n";

    for(const Parameter& par : to_write.parameters) stream << "\t" << par.first << " : " << par.second << "\n";

    return stream << "}";
}
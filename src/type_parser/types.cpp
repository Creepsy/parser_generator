#include "types.h"

#include <cstddef>

using namespace type_parser;

//helper functions
std::ostream& write_parameters(std::ostream& stream, const std::vector<Parameter>& to_write, const size_t indentation_level = 1);

std::ostream& write_parameters(std::ostream& stream, const std::vector<Parameter>& to_write, const size_t indentation_level) {
    for(const Parameter& par : to_write) 
        stream << std::string(indentation_level, '\t') << par.first << " : " << par.second << "\n";

    return stream;
}



TypeInfoTable type_parser::construct_type_info_table(const std::vector<TypeDefinition>& types) {
    TypeInfoTable table;

    for(const TypeDefinition& type : types) {
        table.insert(std::make_pair(type.identifier, TypeInfo{{}, false, type.parameters}));

        for(const std::string& base_type : type.base_types) {
            if(!table.contains(base_type)) 
                table.insert(std::make_pair(base_type, TypeInfo{{}, true, {}}));

            table[base_type].possible_types.insert(type.identifier);
        }
    }

    return table;
}

std::ostream& type_parser::operator<<(std::ostream& stream, const TypeDefinition& to_write) {
    stream << to_write.identifier << "( ";
    for(const std::string& base : to_write.base_types) 
        stream << base << " ";
    stream << ") {\n";

    return write_parameters(stream, to_write.parameters) << "}";
}

std::ostream& type_parser::operator<<(std::ostream& stream, const TypeInfo& to_write) {
    stream << "\tPossible types: ";
    for(const std::string& possible_type : to_write.possible_types) 
        stream << possible_type << " ";
    
    stream << "\n\tIs base: " << (to_write.is_base ? "true" : "false") << "\n";

    stream << "\tArguments:\n";

    return write_parameters(stream, to_write.parameters, 2);
}

std::ostream& type_parser::operator<<(std::ostream& stream, const TypeInfoTable& to_write) {
    for(const std::pair<std::string, TypeInfo>& info : to_write) 
        stream << info.first << " ->\n" << info.second; 

    return stream;
}
#pragma once

#include <string>

namespace code_generator {
    struct LexerFileInfo {
        std::string path;
        std::string name;
    };

    struct ParserFileInfo {
        std::string types_path;
        std::string parser_path;
        std::string namespace_name;
        std::string parser_name;
    };
}
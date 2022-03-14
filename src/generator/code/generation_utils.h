#pragma once

#include <string>

namespace code_generator {
    struct LexerFileInfo {
        std::string path;
        std::string name;
    };

    struct ParserFileInfo {
        std::string output_dir;
        std::string types_name;
        std::string namespace_name;
        std::string parser_name;
    };
}
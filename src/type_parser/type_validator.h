#pragma once

#include "types.h"

namespace type_parser {
    void validate_types(const std::vector<TypeDefinition>& to_validate);
    void validate_type(const TypeDefinition& to_validate, const TypeInfoTable& type_infos);
}
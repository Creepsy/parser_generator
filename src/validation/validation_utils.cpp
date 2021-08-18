#include "validation_utils.h"

#include <stdexcept>

using namespace validation;

void validation::throw_validation_error(const std::string& message) {
    throw std::runtime_error("Validation Error: " + message);
}

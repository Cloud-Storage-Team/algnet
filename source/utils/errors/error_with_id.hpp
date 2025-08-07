#pragma once
#include "base_error.hpp"
#include "utils/identifier_factory.hpp"

namespace utils {

class ErrorWithId : public BaseError {
public:
    ErrorWithId(const BaseError& base_error, Id id);
};

}  // namespace utils
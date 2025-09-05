#include "base_error.hpp"

namespace utils {
BaseError::BaseError(std::string message) : std::runtime_error(message) {}

std::string BaseError::what_str() const { return std::string(what()); }
}  // namespace utils
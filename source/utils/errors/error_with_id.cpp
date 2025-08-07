#include "error_with_id.hpp"

#include <spdlog/fmt/fmt.h>

namespace utils {
ErrorWithId::ErrorWithId(const BaseError& base_error, Id id)
    : BaseError(fmt::format("{}; object id: {}", base_error.what_str(), id)) {}
}  // namespace utils
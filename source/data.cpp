#include "data.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {
Data::Data(DataId a_id, SizeByte a_size) : id(a_id), size(a_size) {}

std::string Data::to_string() const {
    return fmt::format("Data(id = {}; size = {}B)", id, size.value());
}
}  // namespace sim
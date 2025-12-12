#include "data.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {

std::string DataId::to_string() const {
    return fmt::format(
        "(raw id: {}, repeat num: {})", raw_id,
        (repeat_num.has_value() ? std::to_string(repeat_num.value())
                                : "not set"));
}

Data::Data(DataId a_id, SizeByte a_size) : id(std::move(a_id)), size(a_size) {}

std::string Data::to_string() const {
    return fmt::format("Data(id = {}; size = {}B)", id.to_string(),
                       size.value());
}
}  // namespace sim
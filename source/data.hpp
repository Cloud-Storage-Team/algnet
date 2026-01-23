#pragma once
#include <optional>

#include "types.hpp"

namespace sim {

using RepeatNum = std::size_t;
using RawDataId = std::string;

struct DataId {
    RawDataId raw_id;
    std::optional<RepeatNum> repeat_num;

    DataId(RawDataId a_raw_id = "", std::optional<RepeatNum> = std::nullopt);

    auto operator<=>(const DataId&) const = default;

    std::string to_string() const;
};

struct Data {
    DataId id;
    SizeByte size;

    Data(DataId a_id, SizeByte a_size);

    std::string to_string() const;
};

}  // namespace sim

#pragma once
#include <optional>

#include "types.hpp"

namespace sim {

// TODO: use NewDataId instead of DataId
using RepeatNum = std::size_t;
using RawDataId = std::string;

struct NewDataId {
    RawDataId raw_id = {};
    std::optional<RepeatNum> repeat_num = std::nullopt;

    auto operator<=>(const NewDataId&) const = default;

    std::string to_string() const;
};

using DataId = std::string;

struct Data {
    DataId id;
    SizeByte size;

    Data(DataId a_id, SizeByte a_size);

    std::string to_string() const;
};

}  // namespace sim

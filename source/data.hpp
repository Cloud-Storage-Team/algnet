#pragma once
#include "types.hpp"

namespace sim {

using DataId = std::string;

struct Data {
    DataId id;
    SizeByte size;

    Data(DataId a_id, SizeByte a_size);

    std::string to_string() const;
};

}  // namespace sim

#pragma once
#include "types.hpp"

namespace sim {

using DataId = std::uint32_t;

struct Data {
    DataId id = 0;
    SizeByte size;

    inline Data(DataId a_id, SizeByte a_size) : id(a_id), size(a_size) {}
};

}  // namespace sim

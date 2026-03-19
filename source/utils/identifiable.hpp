#pragma once

#include <memory>
#include <vector>

#include "types.hpp"

namespace sim {

class Identifiable {
public:
    virtual ~Identifiable() = default;
    virtual const Id& get_id() const = 0;
};

}  // namespace sim

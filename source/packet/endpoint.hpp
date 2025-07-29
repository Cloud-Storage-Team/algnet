#pragma once
#include <iostream>

#include "types.hpp"

namespace sim {

using Port = std::uint32_t;
struct Endpoint {
    Endpoint(Id a_device_id = "", Port a_port = 0);

    Id device_id;
    Port port;

    bool operator==(const Endpoint& packet) const;
    std::string to_string() const;
};

std::ostream& operator<<(std::ostream& out, const Endpoint& route);
}  // namespace sim
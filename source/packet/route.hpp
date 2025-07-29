#pragma once

#include "endpoint.hpp"

namespace sim {

// 4-tuple that defines route of the packet in the net
struct Route {
    Route(Endpoint a_source = Endpoint(), Endpoint a_dest = Endpoint());
    Route(Id source_id, Id dest_id);

    Endpoint source;
    Endpoint dest;

    bool operator==(const Route& packet) const;
    std::string to_string() const;
};

std::ostream& operator<<(std::ostream& out, const Route& route);

}  // namespace sim
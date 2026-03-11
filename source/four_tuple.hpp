#pragma once
#include <spdlog/fmt/fmt.h>

#include "connection/flow/endpoint_ports.hpp"
#include "types.hpp"

namespace sim {

struct FourTuple : EndpointPorts {
    Id sender_id;
    Id receiver_id;

    FourTuple(Id a_source_id, Id a_dest_id, EndpointPorts ports = {})
        : EndpointPorts(ports),
          sender_id(a_source_id),
          receiver_id(a_dest_id) {}

    auto operator<=>(const FourTuple& tuple) const = default;
};

}  // namespace sim

template <>
struct std::hash<sim::FourTuple> {
    std::size_t operator()(const sim::FourTuple& tuple) const noexcept {
        return std::hash<std::string>()(
            fmt::format("{} {} {} {}", tuple.sender_id, tuple.sender_port,
                        tuple.receiver_id, tuple.receiver_port));
    };
};

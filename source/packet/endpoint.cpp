#include "endpoint.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {

Endpoint::Endpoint(Id a_device_id, Port a_port)
    : device_id(std::move(a_device_id)), port(a_port) {}

bool Endpoint::operator==(const Endpoint& end) const {
    return device_id == end.device_id && port == end.port;
}

std::string Endpoint::to_string() const {
    return fmt::format("{}:{}", device_id, port);
}

std::ostream& operator<<(std::ostream& out, const Endpoint& end) {
    return out << end.to_string();
}

}  // namespace sim
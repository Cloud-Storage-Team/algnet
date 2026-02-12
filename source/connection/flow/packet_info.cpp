#include "packet_info.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {
std::string PacketInfo::to_string() const {
    return fmt::format("(data_id: {}, packet size: {}b, generate time: {}ns",
                       id.to_string(), packet_size.value(),
                       generated_time.value());
}
}  // namespace sim

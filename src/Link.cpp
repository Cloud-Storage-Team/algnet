#include "Link.hpp"

#include <utility>
#include <vector>
#include <memory>

Link::Link(std::shared_ptr<NetworkDevice> source, std::shared_ptr<NetworkDevice> destination, std::uint64_t distance_ns):
        source(std::move(source)),
        destination(std::move(destination)),
        distance_ns(distance_ns) { }

void Link::UpdateLastProcessTime(std::uint64_t new_process_time_ns) {
    last_process_time_ns = std::max(last_process_time_ns, new_process_time_ns);
}

#include "Link.hpp"

#include <utility>
#include <vector>
#include <memory>

Link::Link(std::shared_ptr<NetworkDevice> src, std::shared_ptr<NetworkDevice> dest, std::uint64_t delay_ns):
        src(std::move(src)),
        dest(std::move(dest)),
        delay_ns(delay_ns) { }

void Link::UpdateLastProcessTime(std::uint64_t new_processing_time_ns) {
    last_processing_time_ns = std::max(last_processing_time_ns, new_processing_time_ns);
}

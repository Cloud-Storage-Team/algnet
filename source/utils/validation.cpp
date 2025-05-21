#include "validation.hpp"
#include "logger/logger.hpp"

namespace sim {

bool is_valid_link(std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        LOG_WARN("Passed link is null");
        return false;
    }
    if (link->get_from() == nullptr) {
        LOG_WARN("Link pointer to src device has expired");
        return false;
    }
    if (link->get_to() == nullptr) {
        LOG_WARN("Link pointer to dst device has expired");
        return false;
    }
    return true;
}

}  // namespace sim

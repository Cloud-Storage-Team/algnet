#include "routing_module.hpp"

#include "link.hpp"
#include "logger/logger.hpp"

namespace sim {

bool RoutingModule::add_inlink(std::shared_ptr<ILink> link) {
    if (m_inlinks.contains(link)) {
        LOG_WARN("Unexpected already added inlink");
        return false;
    }
    m_inlinks.insert(link);
    m_next_inlink = LoopIterator<std::set<std::shared_ptr<ILink>>::iterator>(
        m_inlinks.begin(), m_inlinks.end());
    return true;
}

bool RoutingModule::add_outlink(std::shared_ptr<ILink> link) {
    if (m_outlinks.contains(link)) {
        LOG_WARN("Unexpected already added outlink");
        return false;
    }
    m_outlinks.insert(link);
    return true;
}

bool RoutingModule::update_routing_table(std::shared_ptr<IRoutingDevice> dest,
                                         std::shared_ptr<ILink> link) {
    if (link == nullptr) {
        LOG_WARN("Unexpected nullptr link");
        return false;
    }
    auto link_dest = link->get_to();

    // TODO: discuss storing weak_ptrs instead of shared
    m_routing_table[dest] = link;
    return true;
}

std::shared_ptr<ILink> RoutingModule::get_link_to_destination(
    std::shared_ptr<IRoutingDevice> device) const {
    auto iterator = m_routing_table.find(device);
    if (iterator == m_routing_table.end()) {
        return nullptr;
    }

    return (*iterator).second;
}

std::shared_ptr<ILink> RoutingModule::next_inlink() {
    if (m_inlinks.empty()) {
        LOG_INFO("Inlinks storage is empty");
        return nullptr;
    }

    return *m_next_inlink++;
}

std::set<std::shared_ptr<ILink>> RoutingModule::get_outlinks() const {
    return m_outlinks;
}

}  // namespace sim

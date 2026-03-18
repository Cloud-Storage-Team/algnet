#include "routing_module.hpp"

#include <algorithm>

#include "../hashers/ecmp_hasher.hpp"
#include "logger/logger.hpp"
#include "topology/link/i_link.hpp"
#include "utils/validation.hpp"

namespace sim {

RoutingModule::RoutingModule(Id a_id, std::unique_ptr<IPacketHasher> a_hasher)
    : m_id(a_id),
      m_hasher(a_hasher ? std::move(a_hasher)
                        : std::make_unique<ECMPHasher>()) {}

Id RoutingModule::get_id() const { return m_id; }

bool RoutingModule::add_inlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }

    if (m_id != link->get_to()->get_id()) {
        LOG_WARN(
            "Link destination device is incorrect (expected current device)");
        return false;
    }
    if (m_inlinks.contains(link)) {
        LOG_WARN("Unexpected already added inlink");
        return false;
    }
    m_inlinks.insert(link);
    m_next_inlink =
        LoopIterator<std::set<std::weak_ptr<ILink>,
                              std::owner_less<std::weak_ptr<ILink>>>::iterator>(
            m_inlinks.begin(), m_inlinks.end());
    return true;
}

bool RoutingModule::add_outlink(std::shared_ptr<ILink> link) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (m_id != link->get_from()->get_id()) {
        LOG_WARN("Outlink source is not our device");
        return false;
    }
    if (m_outlinks.contains(link)) {
        LOG_WARN("Unexpected already added outlink");
        return false;
    }
    m_outlinks.insert(link);
    return true;
}

bool RoutingModule::update_routing_table(Id receiver_id,
                                         std::shared_ptr<ILink> link,
                                         size_t paths_count) {
    if (!is_valid_link(link)) {
        return false;
    }
    if (m_id != link->get_from()->get_id()) {
        LOG_WARN("Link source device is incorrect (expected current device)");
        return false;
    }
    if (link == nullptr) {
        LOG_WARN("Unexpected nullptr link");
        return false;
    }
    auto link_dest = link->get_to();

    m_routing_table[receiver_id].add_link(link, paths_count);
    return true;
}

std::shared_ptr<ILink> RoutingModule::get_link_to_destination(
    const Packet& packet) const {
    auto iterator = m_routing_table.find(packet.receiver_id);
    if (iterator == m_routing_table.end()) {
        return nullptr;
    }

    const auto& ecmp_next_hops = iterator->second;

    std::uint32_t hash =
        m_hasher->get_hash(packet) % ecmp_next_hops.get_max_hash();

    return ecmp_next_hops[hash];
}

std::shared_ptr<ILink> RoutingModule::next_inlink() {
    if (m_inlinks.empty()) {
        LOG_INFO("Inlinks storage is empty");
        return nullptr;
    }
    auto inlink = *m_next_inlink++;
    if (inlink.expired()) {
        correctify_inlinks();
        return next_inlink();
    }
    return inlink.lock();
}

std::set<std::shared_ptr<ILink>> RoutingModule::get_outlinks() {
    correctify_outlinks();
    std::set<std::shared_ptr<ILink>> shared_outlinks;
    std::transform(m_outlinks.begin(), m_outlinks.end(),
                   std::inserter(shared_outlinks, shared_outlinks.begin()),
                   [](auto link) { return link.lock(); });
    return shared_outlinks;
}

void RoutingModule::correctify_inlinks() {
    std::size_t erased_count = std::erase_if(
        m_inlinks, [](std::weak_ptr<ILink> link) { return link.expired(); });
    if (erased_count > 0) {
        m_next_inlink = LoopIterator(m_inlinks.begin(), m_inlinks.end());
    }
}

void RoutingModule::correctify_outlinks() {
    std::erase_if(m_outlinks,
                  [](std::weak_ptr<ILink> link) { return link.expired(); });
}

}  // namespace sim

#include "routing_module.hpp"

#include <algorithm>
#include <memory>

#include "link.hpp"
#include "logger/logger.hpp"

namespace sim {

bool RoutingModule::add_inlink(std::shared_ptr<ILink> link) {
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

    return (*iterator).second.lock();
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
    std::set<std::weak_ptr<ILink>, std::owner_less<std::weak_ptr<ILink>>>
        correct_inlinks;
    std::copy_if(m_inlinks.begin(), m_inlinks.end(),
                 std::inserter(correct_inlinks, correct_inlinks.begin()),
                 [](std::weak_ptr<ILink> link) { return !link.expired(); });
    m_inlinks.swap(correct_inlinks);
    m_next_inlink = LoopIterator(m_inlinks.begin(), m_inlinks.end());
}

void RoutingModule::correctify_outlinks() {
    std::set<std::weak_ptr<ILink>, std::owner_less<std::weak_ptr<ILink>>>
        correct_outlinks;
    std::copy_if(m_outlinks.begin(), m_outlinks.end(),
                 std::inserter(correct_outlinks, correct_outlinks.begin()),
                 [](std::weak_ptr<ILink> link) { return !link.expired(); });
    m_outlinks.swap(correct_outlinks);
}

}  // namespace sim

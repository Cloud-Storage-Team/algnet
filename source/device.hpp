#pragma once

#include <set>
#include <unordered_map>

#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"

namespace sim {

class Device {
public:
    // BFS to update the routing table
    void recalculate_paths();

    // Different process implemntations in switch/sender/receiver
    virtual void process();

private:
    // Ordered set as we need to iterate over the ingress buffers
    std::set<Link*> m_inlinks;

    // Iterator for the next ingress to process
    std::set<Link>::iterator m_next_inlink;

    // A routing table: maps destination node to a specific link
    std::unordered_map<Device*, Link*> m_outlinks;
};

}  // namespace sim

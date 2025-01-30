#pragma once

#include <any>
#include <queue>

#include "flow.hpp"
#include "node.hpp"
#include "packet.hpp"

namespace sim {

// Base class for event
struct Event {
    int time;
    virtual ~Event();
    virtual void operator()() = 0;
};

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing and dispatch.
 * Schedule the next packet generation event.
 */
struct Generate : public Event {
    Generate(Flow *a_flow);
    Flow *flow;

    virtual void operator()() final;
};

/**
 * Enqueue the packet into the node ingress buffer.
 * Schedule the Dequeue event for the packet.
 */
struct Enqueue : public Event {
    Enqueue(Node *a_node, Packet *a_packet);
    Node *node;
    Packet *packet;

    virtual void operator()() final;
};

/**
 * Dequeue a packet from the node buffer. Find the next hop
 * and pass the packet to the corresponding link for processing
 */
struct Dequeue : public Event {
    Dequeue(Node *a_node);
    Node *node;

    virtual void operator()() final;
};

}  // namespace sim

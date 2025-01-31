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
 * Enqueue the packet to the node for processing.
 */
struct Enqueue : public Event {
    Enqueue(Node *a_node, Packet *a_packet);
    Node *node;
    Packet *packet;

    virtual void operator()() final;
};

/**
 * Dequeue a packet from the node ingress buffer.
 * Pass the packet to the egress buffer (link).
 */
struct Dequeue : public Event {
    Dequeue(Node *a_node);
    Node *node;

    virtual void operator()() final;
};

}  // namespace sim

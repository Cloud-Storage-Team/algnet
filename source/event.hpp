#pragma once
#include <cstdint>

#include "device.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"

namespace sim {

// Base class for event
struct Event {
    std::uint32_t time;
    virtual ~Event();
    virtual void operator()() = 0;
    bool operator>(const Event &other) const { return time > other.time; }
};

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
struct Generate : public Event {
    Generate(Flow *a_flow, std::uint32_t a_packet_size);
    Flow *flow;

    virtual void operator()() final;
};

/**
 * Enqueue the packet to the ingress port of the next node
 */
struct Arrive : public Event {
    Arrive(Link *a_link, Packet *a_packet);
    Link *link;
    Packet *packet;

    virtual void operator()() final;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
struct Process : public Event {
    Process(RountingModule *a_device);
    RountingModule *node;

    virtual void operator()() final;
};

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
struct Stop : public Event {
    virtual void operator()() final;
};

}  // namespace sim

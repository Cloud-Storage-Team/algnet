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
    virtual ~Event() = default;
    virtual void operator()() = 0;
    bool operator>(const Event &other) const { return time > other.time; }
};

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
struct Generate : public Event {
    Generate(std::uint32_t a_time, std::weak_ptr<IFlow> a_flow);
    ~Generate() = default;
    std::uint32_t m_time;
    std::weak_ptr<IFlow> m_flow;

    virtual void operator()() final;
};

/**
 * Enqueue the packet to the ingress port of the next node
 */
struct Arrive : public Event {
    // TODO: move implementation to .cpp or use existing if present
    Arrive(ILink *a_link, Packet *a_packet) : link(a_link), packet(a_packet){};
    ~Arrive() = default;
    ILink *link;
    Packet *packet;

    virtual void operator()() final { link->process_arrival(*packet); };
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
struct Process : public Event {
    Process(IProcessingDevice *a_device);
    IProcessingDevice *node;

    virtual void operator()() final;
};

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
struct Stop : public Event {
    Stop();
    ~Stop() = default;
    virtual void operator()() final;
};

}  // namespace sim

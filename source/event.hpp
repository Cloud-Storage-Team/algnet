#pragma once
#include <cstdint>

#include "device.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"

namespace sim {

// Base class for event
class Event {
public:
    Event(std::uint32_t a_time);
    virtual ~Event() = default;
    virtual void operator()() = 0;

    std::uint32_t get_time() const;

protected:
    std::uint32_t m_time;
};

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
class Generate : public Event {
    Generate(std::uint32_t a_time, Flow *a_flow, std::uint32_t a_packet_size);
    ~Generate() = default;
    virtual void operator()() final;

private:
    Flow *m_flow;
    std::uint32_t m_packet_size;
};

/**
 * Enqueue the packet to the ingress port of the next node
 */
class Arrive : public Event {
    Arrive(std::uint32_t a_time, Link *a_link, Packet a_packet);
    ~Arrive() = default;
    virtual void operator()() final;

private:
    Link *m_link;
    Packet m_packet;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class Process : public Event {
    Process(std::uint32_t a_time, Device *a_device);
    ~Process() = default;
    virtual void operator()() final;

private:
    Device *m_device;
};

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
class Stop : public Event {
public:
    Stop(std::uint32_t a_time);
    ~Stop() = default;
    virtual void operator()() final;
};

}  // namespace sim

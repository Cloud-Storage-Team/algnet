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
    bool operator>(const Event &other) const { return m_time > other.m_time; }

protected:
    std::uint32_t m_time;
};

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
struct Generate : public Event {
    Generate(Flow *a_flow, std::uint32_t a_packet_size);
    ~Generate() = default;
    Flow *flow;

    virtual void operator()() final;
};

/**
 * Enqueue the packet to the ingress port of the next node
 */
struct Arrive : public Event {
    // TODO: move implementation to .cpp or use existing if present
    Arrive(std::uint32_t a_time, ILink *a_link, Packet *a_packet) : link(a_link), packet(a_packet), Event(a_time) {};
    ~Arrive() = default;
    ILink *link;
    Packet *packet;

    virtual void operator()() final { link->process_arrival(*packet); };
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class Process : public Event {
public:
    // TODO: move implementation to .cpp or use existing if present
    Process(std::uint32_t a_time, IProcessingDevice *a_device);
    ~Process() = default;
    virtual void operator()() final;

private:
    IProcessingDevice *m_device;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class SendData : public Event {
public:
    // TODO: move implementation to .cpp or use existing if present
    SendData(std::uint32_t a_time, ISender *a_device);
    ~SendData() = default;
    virtual void operator()() final;

private:
ISender *m_device;
};

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
struct Stop : public Event {
    Stop(std::uint32_t a_time);
    ~Stop() = default;
    virtual void operator()() final;
};

}  // namespace sim

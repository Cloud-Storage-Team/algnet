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
class Generate : public Event {
public:
    Generate(std::weak_ptr<Flow> a_flow, std::uint32_t a_packet_size);
    virtual ~Generate() = default;
    void operator()() final;

private:
    std::weak_ptr<Flow> m_flow;
};

/**
 * Enqueue the packet to the ingress port of the next node
 */
class Arrive : public Event {
public:
    Arrive(std::uint32_t a_time, ILink *a_link, Packet a_packet);
    virtual ~Arrive() = default;

    void operator()() final;

private:
    // TODO: use weak_ptr (requires enable_from_this implementation for some links)
    ILink *m_link;
    Packet m_packet;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class Process : public Event {
public:
    // TODO: move implementation to .cpp or use existing if present
    Process(std::uint32_t a_time, std::weak_ptr<IProcessingDevice> a_device);
    ~Process() = default;
    void operator()() final;

private:
    std::weak_ptr<IProcessingDevice> m_device;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class SendData : public Event {
public:
    // TODO: move implementation to .cpp or use existing if present
    SendData(std::uint32_t a_time, std::weak_ptr<ISender> _device);
    ~SendData() = default;
    void operator()() final;

private:
    std::weak_ptr<ISender> m_device;
};

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
class Stop : public Event {
public:
    Stop(std::uint32_t a_time);
    virtual ~Stop() = default;
    void operator()() final;
};

}  // namespace sim

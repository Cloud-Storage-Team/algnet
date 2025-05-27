#pragma once

#include "device/device.hpp"
#include "flow.hpp"
#include "link.hpp"
#include "packet.hpp"
#include "types.hpp"

namespace sim {

// Base class for event
class Event {
public:
    Event(Time a_time);
    virtual ~Event() = default;
    virtual void operator()() = 0;

    Time get_time() const;
    bool operator>(const Event &other) const { return m_time > other.m_time; }

protected:
    Time m_time;
};

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
class Generate : public Event {
public:
    Generate(Time a_time, std::weak_ptr<IFlow> a_flow, Size a_packet_size);
    virtual ~Generate() = default;
    void operator()() final;

private:
    std::weak_ptr<IFlow> m_flow;
    Size m_packet_size;
};

/**
 * Enqueue the packet to the ingress port of the next node
 */
class Arrive : public Event {
public:
    Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet);
    virtual ~Arrive() = default;

    void operator()() final;

private:
    std::weak_ptr<ILink> m_link;
    Packet m_packet;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class Process : public Event {
public:
    Process(Time a_time, std::weak_ptr<IProcessingDevice> a_device);
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
    SendData(Time a_time, std::weak_ptr<ISender> a_device);
    ~SendData() = default;
    void operator()() final;

private:
    std::weak_ptr<ISender> m_device;
};


/**
 * Run new flow at specified time
 */
class StartFlow : public Event {
public:
    StartFlow(Time a_time, std::weak_ptr<IFlow> a_flow);
    ~StartFlow() = default;
    void operator()() final;

private:
    std::weak_ptr<IFlow> m_flow;
};

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
class Stop : public Event {
public:
    Stop(Time a_time);
    virtual ~Stop() = default;
    void operator()() final;
};

}  // namespace sim

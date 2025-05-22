#pragma once

#include <variant>

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
    bool operator>(const Event& other) const { return m_time > other.m_time; }

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
 * Stop simulation and clear all events remaining in the Scheduler
 */
class Stop : public Event {
public:
    Stop(Time a_time);
    virtual ~Stop() = default;
    void operator()() final;
};

struct BaseEvent {
    BaseEvent(const Generate& e) : event(e) {}
    BaseEvent(const Arrive& e) : event(e) {}
    BaseEvent(const Process& e) : event(e) {}
    BaseEvent(const SendData& e) : event(e) {}
    BaseEvent(const Stop& e) : event(e) {}

    void operator()() {
        std::visit([&](auto real_event) { real_event(); }, event);
    }
    bool operator>(const BaseEvent& other) const {
        return this->get_time() > other.get_time();
    }
    bool operator<(const BaseEvent& other) const {
        return this->get_time() < other.get_time();
    }
    Time get_time() const {
        return std::visit(
            [&](auto real_event) { return real_event.get_time(); }, event);
    }

    std::variant<Generate, Arrive, Process, SendData, Stop> event;
};

}  // namespace sim

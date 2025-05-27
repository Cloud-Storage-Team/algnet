#pragma once

#include <variant>

#include "device/device.hpp"
#include "flow/flow.hpp"
#include "link.hpp"
#include "packet.hpp"
#include "types.hpp"

namespace sim {

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
class Generate {
public:
    Generate(Time a_time, std::weak_ptr<IFlow> a_flow, Size a_packet_size);
    ~Generate() = default;
    void operator()();

    Time get_time() const;

private:
    std::weak_ptr<IFlow> m_flow;
    Size m_packet_size;
    Time m_time;
};

/**
 * Enqueue the packet to the ingress port of the next node
 */
class Arrive {
public:
    Arrive(Time a_time, std::weak_ptr<ILink> a_link, Packet a_packet);
    ~Arrive() = default;
    void operator()();

    Time get_time() const;

private:
    std::weak_ptr<ILink> m_link;
    Packet m_packet;
    Time m_time;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class Process {
public:
    Process(Time a_time, std::weak_ptr<IProcessingDevice> a_device);
    ~Process() = default;
    void operator()();

    Time get_time() const;

private:
    std::weak_ptr<IProcessingDevice> m_device;
    Time m_time;
};

/**
 * Dequeue a packet from the device ingress buffer
 * and start processing at the device.
 */
class SendData {
public:
    SendData(Time a_time, std::weak_ptr<ISender> a_device);
    ~SendData() = default;
    void operator()();

    Time get_time() const;

private:
    std::weak_ptr<ISender> m_device;
    Time m_time;
};

/**
 * Run new flow at specified time
 */
class StartFlow {
public:
    StartFlow(Time a_time, std::weak_ptr<IFlow> a_flow);
    ~StartFlow() = default;
    void operator()();

    Time get_time() const;

private:
    std::weak_ptr<IFlow> m_flow;
    Time m_time;
};

/**
 * Stop simulation and clear all events remaining in the Scheduler
 */
class Stop {
public:
    Stop(Time a_time);
    ~Stop() = default;
    void operator()();

    Time get_time() const;

private:
    Time m_time;
};

struct BaseEvent {
    BaseEvent(const Generate& e);
    BaseEvent(const Arrive& e);
    BaseEvent(const Process& e);
    BaseEvent(const SendData& e);
    BaseEvent(const Stop& e);
    BaseEvent(const StartFlow& e);

    void operator()();
    bool operator>(const BaseEvent& other) const;
    Time get_time() const;

    std::variant<Generate, Arrive, Process, SendData, Stop, StartFlow> event;
};

}  // namespace sim

#pragma once

namespace sim {

class Flow;
class Device;
class Link;
class Packet;

// Base class for event
struct Event {
    int time;
    virtual ~Event();
    virtual void operator()() = 0;
};

/**
 * Generate a packet in the flow and enqueue it in
 * the source node ingress buffer for processing.
 * Schedule the next packet generation event.
 */
struct Generate : public Event {
    Generate(Flow *a_flow, int a_packet_size);
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
    Process(Device *a_device);
    Device *node;

    virtual void operator()() final;
};

}  // namespace sim

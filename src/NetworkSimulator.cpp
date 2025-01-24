#include "NetworkSimulator.hpp"
#include "Flow.hpp"

void NetworkSimulator::Run(std::uint64_t time_ns) {
    stop_time_ns = time_ns;
    event_scheduler = std::make_unique<EventScheduler>();

    /* Start packet sending from each sender */
    for (std::shared_ptr<Flow>& flow: flows) {
        flow->Send();
    }

    /* Process events */
    while (!event_scheduler->Empty() && event_scheduler->PeekNextEventTime() < stop_time_ns) {
        Event e = event_scheduler->PopNextEvent();
        current_time_ns = e.execution_time;
        e.Execute();
    }
}

void NetworkSimulator::AddDevice(std::shared_ptr<NetworkDevice> device) {
    device_by_id[device->id] = device;
}

void NetworkSimulator::AddLink(std::shared_ptr<Link> link, std::shared_ptr<Flow> flow) {
    std::shared_ptr<Link> reversed_link = std::make_shared<Link>(link->dest, link->src, link->delay_ns);
    link->src->routing_table[flow->getReceiverID()] = link;
    link->dest->routing_table[flow->getSenderID()] = reversed_link;
}

void NetworkSimulator::AddFlow(std::shared_ptr<Flow> flow) {
    flows.push_back(flow);
}

void NetworkSimulator::Schedule(std::uint64_t delay, const std::function<void()>& handler) {
    event_scheduler->Schedule(current_time_ns + delay, handler);
}

std::uint64_t NetworkSimulator::Now() {
    return current_time_ns;
}

std::uint64_t Time::Seconds(std::uint64_t time_s) {
    return time_s * s_to_ns;
}

std::uint64_t Time::Milliseconds(std::uint64_t time_ms) {
    return time_ms * ms_to_ns;
}

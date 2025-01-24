#include "NetworkSimulator.hpp"
#include "Flow.hpp"

void NetworkSimulator::Run() const {
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

void NetworkSimulator::StopAt(std::uint64_t time_ns) {
    stop_time_ns = time_ns;
}

void NetworkSimulator::AddDevice(std::shared_ptr<NetworkDevice> device) {
    device_by_id[device->id] = device;
}

void NetworkSimulator::AddLink(std::shared_ptr<Link> link, std::shared_ptr<Flow> flow) {
    std::shared_ptr<Link> reversed_link = std::make_shared<Link>(link->destination, link->source, link->distance_ns);
    // pair <current_device, receiver_id> --> link to next device
    routing_table[{link->source->id, flow->m_receiver->id}] = link;
    routing_table[{link->destination->id, flow->m_sender->id}] = reversed_link;
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

void NetworkSimulator::AddNewFlow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver, std::vector<std::shared_ptr<Switch>>& switches) {
    for (auto& s : switches) {
        if (device_by_id.find(s->id) == device_by_id.end()) {
            AddDevice(s);
        }
    }
    if (device_by_id.find(sender->id) == device_by_id.end()) {
        AddDevice(sender);
    }
    if (device_by_id.find(receiver->id) == device_by_id.end()) {
        AddDevice(receiver);
    }

    std::shared_ptr<Flow> flow = std::make_shared<Flow>(sender, receiver, 10000); //  TODO add param
    AddFlow(flow);


    std::shared_ptr<NetworkDevice> prev_device = sender;
    for (auto& s : switches) {
        std::shared_ptr<Link> link = std::make_shared<Link>(prev_device, s, 10); //  TODO add param or random
        AddLink(link, flow);
        prev_device = s;
    }
    std::shared_ptr<Link> link = std::make_shared<Link>(prev_device, receiver, 10); //  TODO add param
    AddLink(link, flow);

}

void NetworkSimulator::GenerateTree(std::shared_ptr<NetworkDevice> receiver, int maxDepth, int k, std::vector<std::shared_ptr<Switch>>& currentSwitches, int currentDepth){
    if (currentDepth == maxDepth) {
        std::shared_ptr<Sender> sender = std::make_shared<Sender>(100); //  TODO add param or random
        AddNewFlow(sender, receiver, currentSwitches);
        return;
    }

    std::shared_ptr<Switch> sw = std::make_shared<Switch>(100); //  TODO add param or random
    currentSwitches.push_back(sw);

    for (int i=0; i<k; i++){
        GenerateTree(receiver, maxDepth, k, currentSwitches, currentDepth + 1);
    }

    currentSwitches.pop_back();
}

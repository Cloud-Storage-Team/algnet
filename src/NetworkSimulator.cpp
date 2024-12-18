#include "NetworkSimulator.hpp"
#include "Flow.hpp"

void NetworkSimulator::Run() const {
    event_scheduler = std::make_unique<EventScheduler>();

    /* Fill link time map */
    for (std::unique_ptr<Flow>& flow: flows) {
        for (std::uint32_t i = 0; i < flow->path.size() - 1; ++i) {
            link_last_process_time_ns[{std::min(flow->path[i]->id, flow->path[i + 1]->id),
                                       std::max(flow->path[i]->id, flow->path[i + 1]->id)}] = 0;
        }
    }

    /* Create routing table */
    for (std::unique_ptr<Flow>& flow: flows) {
        for (std::uint32_t i = 0; i < flow->path.size() - 1; ++i) {
            if (routing_table.count(flow->path[i]->id) == 0) {
                routing_table[flow->path[i]->id] = device_by_id[flow->path[i + 1]->id];
            }
        }
    }
    /* Start packet sending from each sender */
    for (std::unique_ptr<Flow>& flow: flows) {
        flow->Send();
    }

    /* Process events */
    while (!event_scheduler->Empty() && event_scheduler->PeekNextEventTime() < stop_time_ns) {
        Event e = event_scheduler->PopNextEvent();
        current_time_ns = e.GetTime();
        e.Execute();
    }
}

void NetworkSimulator::StopAt(double time_ns) {
    stop_time_ns = time_ns;
}

void NetworkSimulator::AddDevice(std::shared_ptr<NetworkDevice> device) {
    device_by_id[device->id] = device;
}

void NetworkSimulator::EmplaceFlow(const std::vector<std::shared_ptr<NetworkDevice>>& path,
                                   const std::vector<std::uint32_t>& distances_ns) {
    flows.emplace_back(new Flow(path, distances_ns));
}

void NetworkSimulator::Schedule(double delay, const std::function<void()>& handler) {
    event_scheduler->Schedule(current_time_ns + delay, handler);
}

double NetworkSimulator::Now() {
    return current_time_ns;
}

double Time::Seconds(double time_s) {
    return time_s * s_to_ns;
}

#include "NetworkSimulator.hpp"
#include "Flow.hpp"

void NetworkSimulator::Run() const {
    event_scheduler = std::make_unique<EventScheduler>();

    /* Fill link time map and distances map*/
    for (std::unique_ptr<Flow>& flow: flows) {
        for (std::uint32_t i = 0; i < flow->path.size() - 1; ++i) {
            /* Forward link */
            link_last_process_time_ns[{flow->path[i]->id, flow->path[i + 1]->id}] = 0;
            /* Backward link */
            link_last_process_time_ns[{flow->path[i + 1]->id, flow->path[i]->id}] = 0;

            distances_ns[{std::min(flow->path[i]->id, flow->path[i + 1]->id),
                          std::max(flow->path[i]->id, flow->path[i + 1]->id)}] = flow->distances_ns[i];
        }
    }

    /* Create routing tables */
    for (std::unique_ptr<Flow>& flow: flows) {
        for (std::uint32_t i = 0; i < flow->path.size() - 1; ++i) {
            if (forward_routing_table.count(flow->path[i]->id) == 0) {
                forward_routing_table[flow->path[i]->id] = device_by_id[flow->path[i + 1]->id];
            }
            if (backward_routing_table.count(flow->path[i + 1]->id) == 0) {
                backward_routing_table[flow->path[i + 1]->id] = device_by_id[flow->path[i]->id];
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

void NetworkSimulator::StopAt(std::uint64_t time_ns) {
    stop_time_ns = time_ns;
}

void NetworkSimulator::AddDevice(std::shared_ptr<NetworkDevice> device) {
    device_by_id[device->id] = device;
}

void NetworkSimulator::EmplaceFlow(const std::vector<std::shared_ptr<NetworkDevice>>& flow_path,
                                   const std::vector<std::uint32_t>& flow_distances_ns,
                                   std::uint64_t flow_packet_generation_interval_ns) {
    flows.emplace_back(new Flow(flow_path, flow_distances_ns, flow_packet_generation_interval_ns));
}

void NetworkSimulator::Schedule(std::uint64_t delay, const std::function<void()>& handler) {
    event_scheduler->Schedule(current_time_ns + delay, handler);
}

std::uint64_t NetworkSimulator::Now() {
    return current_time_ns;
}

std::uint32_t NetworkSimulator::GetDistanceNs(std::uint32_t src_id, std::uint32_t dst_id) {
    return NetworkSimulator::distances_ns[{std::min(src_id, dst_id), std::max(src_id, dst_id)}];
}

std::uint64_t NetworkSimulator::GetLinkLastProcessTime(std::uint32_t src_id, std::uint32_t dst_id) {
    return NetworkSimulator::link_last_process_time_ns[{src_id, dst_id}];
}

void NetworkSimulator::UpdateLinkLastProcessTime(std::uint32_t src_id, std::uint32_t dst_id, std::uint64_t value) {
    NetworkSimulator::link_last_process_time_ns[{src_id, dst_id}] = value;
}

std::uint64_t Time::Seconds(std::uint64_t time_s) {
    return time_s * s_to_ns;
}

std::uint64_t Time::Milliseconds(std::uint64_t time_ms) {
    return time_ms * ms_to_ns;
}

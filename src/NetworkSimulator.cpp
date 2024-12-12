#include "NetworkSimulator.hpp"
#include "Flow.hpp"

#include <iostream>

void NetworkSimulator::Run() {
    /* Create map: id --> device */
    for (std::uint32_t i = 0; i < devices.size(); ++i) {
        device_index_by_id[devices[i]->id] = i;
    }
    /* Fill link map with keys */
    for (std::unique_ptr<Flow>& flow: flows) {
        for (std::uint32_t i = 0; i < flow->path.size() - 1; ++i) {
            link_last_process_time_ns[{std::min(flow->path[i]->id, flow->path[i + 1]->id),
                                       std::max(flow->path[i]->id, flow->path[i + 1]->id)}] = 0;
        }
    }

    for (std::unique_ptr<Flow>& flow: flows) {
        flow->Send();
    }
    ProcessEvents();
}

void NetworkSimulator::ProcessEvents() {
    while (!NetworkSimulator::event_scheduler.empty()) {
        /* Take event from the top of priority queue */
        const Event& event = NetworkSimulator::event_scheduler.top();
        /* Call `Send` method from the receiver of the event */
        devices[device_index_by_id[event.receiver_id]]->Send(event.flow_id);
        /* Remove processed event from priority_queue */
        NetworkSimulator::event_scheduler.pop();
    }
}


#include "Switch.hpp"
#include "NetworkSimulator.hpp"

Switch::Switch():
    NetworkDevice(DeviceType::Switch) {
    id = NetworkDevice::last_given_device_id++;
}

void Switch::Send(std::uint32_t flow_id) {
    const Event& event = NetworkSimulator::event_scheduler.top();

    /* Searching for flow_id in table */
    auto table_it = switching_table.find(flow_id);
    if (table_it == switching_table.end()) {
        /* Add key: flow_id and value: <id of the next device in flow path, distance to it> */
        std::tuple<std::uint32_t, std::uint64_t, std::uint32_t, std::uint64_t> neighbor_devices = NetworkSimulator::flows[flow_id]->FindAdjDevicesInPath(
                id);
        switching_table[flow_id] = neighbor_devices;
    }

    std::uint32_t receiver_id;
    std::uint64_t distance_ns;
    if (event.is_ACK) {
        receiver_id = std::get<Flow::PrevID>(switching_table[flow_id]);
        distance_ns = std::get<Flow::PrevDist>(switching_table[flow_id]);
    }
    else {
        receiver_id = std::get<Flow::NextID>(switching_table[flow_id]);
        distance_ns = std::get<Flow::NextDist>(switching_table[flow_id]);
    }

    std::uint64_t link_last_process_time = NetworkSimulator::link_last_process_time_ns[{std::min(id, receiver_id), std::max(id, receiver_id)}];
    /* New event delivery time */
    std::uint64_t event_delivery_time = std::max(link_last_process_time, event.delivery_time_ns) + distance_ns;
    /* Send event to the next device */
    NetworkSimulator::event_scheduler.emplace(event.packet, flow_id, receiver_id, event_delivery_time, event.is_ACK);
    /* Update last link process time */
    NetworkSimulator::link_last_process_time_ns[{std::min(id, receiver_id), std::max(id, receiver_id)}] = event_delivery_time;
}

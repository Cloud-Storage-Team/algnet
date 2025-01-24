
#include "express_pass_server.hpp"

ExpressPassReceiver::ExpressPassReceiver(std::vector<std::uint64_t> senders_ids, std::uint64_t id, std::uint64_t simulation_duration, std::uint64_t inter_credit_gap):
    ServerBase(id),
    senders_ids(senders_ids),
    inter_credit_gap(inter_credit_gap),
    simulation_duration(simulation_duration) {}

std::uint64_t ExpressPassReceiver::SendCreditByFlow(std::uint64_t flow_id, std::uint64_t current_time_ns, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) {
    std::uint64_t jitter = congestion_control.getRandomJitter();
    std::uint64_t sending_time = current_time_ns + jitter;
    PacketHeader credit = congestion_control.GetCredit(sending_time, id, flow_id, ++flow_to_index[flow_id].last_sent_index);
    // std::cout << "Flow id: " << flow_id << ", last sent: " << flow_to_index[flow_id].last_sent_index << std::endl;
    this->GetNextElement(flow_id);

    auto process = [this, flow_id, sending_time, credit](std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) {
            auto inner_credit = credit;
            this->GetNextElement(flow_id)->ReceivePacket(sending_time, inner_credit, events);
        };
    all_events.push(std::make_shared<Event>(Event(sending_time, process, flow_id)));

    std::uint64_t next_sending = sending_time + inter_credit_gap;
    if (next_sending < simulation_duration) {
        auto create_credit = [this, flow_id, next_sending](std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) {
                this->SendCreditByFlow(flow_id, next_sending, events);
            };
        all_events.push(std::make_shared<Event>(Event(next_sending, create_credit)));
    }

    return next_sending;
}

std::uint64_t ExpressPassReceiver::SendPackets(std::uint64_t current_time_ns, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) {
    if (current_time_ns == 0) {
        // TODO: On 0 iteration should initialize all flows
        // return;
    }
    // TODO: Log each ask for update
    std::uint64_t next_sending = std::numeric_limits<uint64_t>::max();
    for (auto destination_id : senders_ids) {
        SendCreditByFlow(destination_id, current_time_ns, all_events);
    }

    return next_sending;
}

void ExpressPassReceiver::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) {
    if (packet.GetFlag(PacketType::IsAck) == 1) {
        // TODO: On 0 iteration we just remember that flow is now active
        return;
    }
    std::uint64_t sender_id = packet.source_id;
    flow_to_lost_credits[sender_id] += (packet.packet_index - flow_to_index[sender_id].last_received_index) - 1;
    flow_to_index[sender_id].last_received_index = packet.packet_index;
    // std::cout << "Flow id: " << sender_id << ", receive obtained: " << flow_to_index[sender_id].last_received_index << std::endl;
    // std::cout << "Lost rate: " << flow_to_lost_credits[sender_id] << "/" << flow_to_index[sender_id].last_received_index << std::endl;
    rtt = static_cast<std::uint64_t>(rtt * 0.7 + (current_time_ns - packet.rtt) * 0.3);
    std::cout << "rtt: " << rtt << std::endl;
    if (packet.sending_time <= simulation_duration) {
        amount_of_data_from_sender[sender_id] += 1;
    }
}

std::string ExpressPassReceiver::ToString() const {
        std::ostringstream oss;

        std::vector<std::pair<std::uint64_t, std::uint32_t>> result;
        for (auto& sender_and_data: amount_of_data_from_sender) {
            result.push_back(sender_and_data);
        }
        std::sort(result.begin(), result.end());

        for (auto& sender_and_data: result) {
            oss << "Sender id: "<< sender_and_data.first << ", Amount: " << sender_and_data.second << std::endl;
        }
        std::uint32_t sum = 0;
        for (auto& data: flow_to_lost_credits) {
            sum += data.second;
        }
        oss << sum << std::endl;
        oss << std::endl;

        return oss.str();
}

ExpressPassSender::ExpressPassSender(std::uint64_t receiver_id, std::uint64_t id, std::uint64_t simulation_duration, std::uint64_t process_time_ns, std::uint8_t max_credit_bucket_size):
    ServerBase(id),
    max_credit_bucket_size(max_credit_bucket_size),
    process_time_ns(process_time_ns),
    receiver_id(receiver_id),
    simulation_duration(simulation_duration) {}

std::uint64_t ExpressPassSender::SendPackets(std::uint64_t current_time_ns, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) {
    // if (current_time_ns == 0) {
    //     PacketHeader data = congestion_control.GetHandshakePacket(current_time_ns + process_time_ns, id, receiver_id);
    //     TODO: create sending event
    // }
    return simulation_duration + 1;
}

void ExpressPassSender::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) {
    if (packet.GetFlag(PacketType::IsInitializer) == 1) {
        // TODO: On 0 iteration we send packet with agreement to setup connection
        return;
    }
    else if (packet.GetFlag(PacketType::IsCredit) == 1) {
        PacketHeader data = congestion_control.GetDataPacket(current_time_ns + process_time_ns, id, packet.source_id, packet.packet_index, packet.rtt);
        std::uint64_t process_time = process_time_ns;
        this->GetNextElement(data.destination_id);

        auto process = [this, current_time_ns, process_time, data](std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& events) {
                auto inner_data = data;
                this->GetNextElement(inner_data.destination_id)->ReceivePacket(current_time_ns + process_time_ns, inner_data, events);
            };
        all_events.push(std::make_shared<Event>(Event(current_time_ns + process_time_ns, process, data.destination_id)));
    }
}
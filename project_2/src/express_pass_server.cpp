#include "express_pass_server.hpp"


ExpressPassReceiver::ExpressPassReceiver(std::vector<std::uint64_t> senders_ids, std::uint64_t id, std::uint64_t simulation_duration, std::uint64_t inter_credit_gap):
    ServerBase(id),
    senders_ids(senders_ids),
    inter_credit_gap(inter_credit_gap),
    simulation_duration(simulation_duration) {}

std::uint64_t ExpressPassReceiver::SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) {
    if (current_time_ns == 0) {
        // TODO: On 0 iteration should initialize all flows
        // return;
    }
    // TODO: Log each ask for update
    std::uint64_t next_sending = std::numeric_limits<uint64_t>::max();
    for (auto destination_id : senders_ids) {
        // TODO: Send only for approved flows
        std::uint64_t jitter = congestion_control.getRandomJitter();
        std::uint64_t sending_time = ids_to_sending_time[destination_id] + jitter;
        ids_to_sending_time[destination_id] = sending_time + inter_credit_gap;
        next_sending = std::min(next_sending, ids_to_sending_time[destination_id]);
        // std::cout << "Destination: " << destination_id << ", current time: " << current_time_us << ", jitter: " << jitter << std::endl;
        PacketHeader credit = congestion_control.GetCredit(sending_time, id, destination_id);
        packets_wrapped.push(RoutingPacket(credit, this->GetNextElement(destination_id)));
    }

    return next_sending;
}

void ExpressPassReceiver::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) {
    if (packet.GetFlag(2) == 1) {
        // TODO: On 0 iteration we just remember that flow is now active
        return;
    }
    std::uint64_t sender_id = packet.GetSourceID();
    
    if (packet.GetSendingTime() <= simulation_duration) {
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
        oss << std::endl;

        return oss.str();
}

ExpressPassSender::ExpressPassSender(std::uint64_t id, std::uint64_t process_time_ns, std::uint8_t max_credit_bucket_size):
    ServerBase(id),
    max_credit_bucket_size(max_credit_bucket_size),
    process_time_ns(process_time_ns) {}

std::uint64_t ExpressPassSender::SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) {
    // does nothing
    return 0;
}

void ExpressPassSender::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) {
    if (packet.GetFlag(2) == 1) {
        // TODO: On 0 iteration we send packet with agreement to setup connection
        return;
    }
    else if (packet.GetFlag(0) == 1) {
        PacketHeader data = congestion_control.GetDataPacket(current_time_ns + process_time_ns, id, packet.GetSourceID());
        packets_wrapped.push(RoutingPacket(data, this->GetNextElement(data.GetDestinationID())));
    }
}
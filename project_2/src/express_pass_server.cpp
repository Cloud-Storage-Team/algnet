#include "express_pass_server.hpp"


ExpressPassReciever::ExpressPassReciever(std::vector<std::uint64_t> senders_ids, std::uint64_t id, std::uint64_t inter_credit_gap) {
    this->id = id;
    this->senders_ids = senders_ids;
    this->inter_credit_gap = inter_credit_gap;
}

std::uint64_t ExpressPassReciever::SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) {
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
        // std::cout << sending_time - jitter << std::endl;
        ids_to_sending_time[destination_id] = sending_time + inter_credit_gap;
        next_sending = std::min(next_sending, ids_to_sending_time[destination_id]);
        // std::cout << "Destination: " << destination_id << ", current time: " << current_time_us << ", jitter: " << jitter << std::endl;
        PacketHeader credit = congestion_control.GetCredit(sending_time, id, destination_id);
        packets_wrapped.push(RoutingPacket(credit, this->GetNextElement(destination_id)));
    }

    return next_sending;
    // std::cout << std::endl;
}

void ExpressPassReciever::ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) {
    if (packet.GetFlag(2) == 1) {
        // TODO: On 0 iteration we just remember that flow is now active
        return;
    }
    std::uint64_t sender_id = packet.GetSourceID();
    // TODO: process it more correctly (maybe need to pass simulation limits)
    // if (packet.GetSendingTime() <= 100000000) {
        amount_of_data_from_sender[sender_id] += 1;
    // }

    // TODO: process it more correctly (maybe need to pass simulation limits)
    // if (current_time_ns > 100000000) {
    //     std::vector<std::pair<std::uint64_t, std::uint32_t>> result;
    //     for (auto& sender_and_data: amount_of_data_from_sender) {
    //         result.push_back(sender_and_data);
    //     }
    //     std::sort(result.begin(), result.end());
    //     for (auto& sender_and_data: result) {
    //         std::cout << "Sender id: "<< sender_and_data.first << ", Amount: " << sender_and_data.second << std::endl;
    //     }
    //     std::cout << std::endl;
    // }
}

ExpressPassSender::ExpressPassSender(std::uint64_t id, std::uint64_t process_time_ns, std::uint8_t max_credit_bucket_size) {
    this->id = id;
    this->max_credit_bucket_size = max_credit_bucket_size;
    this->process_time_ns = process_time_ns;
}

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
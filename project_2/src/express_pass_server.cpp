
#include "common_events.hpp"
#include "express_pass_server.hpp"

ExpressPassReceiver::ExpressPassReceiver(std::vector<std::uint64_t> senders_ids, std::uint64_t id, std::uint64_t simulation_duration, std::uint64_t inter_credit_gap):
    ServerBase(id),
    senders_ids(senders_ids),
    inter_credit_gap(inter_credit_gap),
    simulation_duration(simulation_duration) {}

std::uint64_t ExpressPassReceiver::SendCreditByFlow(std::uint64_t flow_id, std::uint64_t current_time_ns, std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, EventComparator>& all_events) {
    std::uint64_t jitter = congestion_control.getRandomJitter();
    std::uint64_t sending_time = current_time_ns + jitter;
    PacketHeader credit = congestion_control.GetCredit(sending_time, id, flow_id);
    all_events.push(std::make_shared<ProcessPacketEvent>(ProcessPacketEvent(this->GetNextElement(flow_id), credit, sending_time, flow_id)));

    std::uint64_t next_sending = sending_time + inter_credit_gap;
    if (next_sending < simulation_duration) {
        all_events.push(std::make_shared<CreateCreditByFlowEvent>(CreateCreditByFlowEvent(std::make_shared<ExpressPassReceiver>(*this), flow_id, next_sending)));
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
    if (packet.GetFlag(2) == 1) {
        // TODO: On 0 iteration we just remember that flow is now active
        return;
    }
    std::uint64_t sender_id = packet.source_id;
    
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
    if (packet.GetFlag(2) == 1) {
        // TODO: On 0 iteration we send packet with agreement to setup connection
        return;
    }
    else if (packet.GetFlag(0) == 1) {
        PacketHeader data = congestion_control.GetDataPacket(current_time_ns + process_time_ns, id, packet.source_id);
        all_events.push(std::make_shared<ProcessPacketEvent>(ProcessPacketEvent(this->GetNextElement(data.destination_id), data, current_time_ns + process_time_ns, data.destination_id)));
    }
}
#include "server.hpp"

ServerReceiver::ServerReceiver(std::vector<std::uint64_t> senders_ids,
                               std::uint64_t id) {
    this->id = id;
    this->senders_ids = senders_ids;
}

std::uint64_t
ServerReceiver::SendPackets(std::uint64_t current_time_ns,
                            PriorityQueueWrapper &packets_wrapped) {

    return 0;
    // std::cout << std::endl;
}

void ServerReceiver::ReceivePacket(std::uint64_t current_time_ns,
                                   PacketHeader &packet,
                                   PriorityQueueWrapper &packets_wrapped) {
    std::uint64_t sender_id = packet.GetSourceID();
    last_process_time_ns =
        std::max(last_process_time_ns, current_time_ns) + process_time_ns;
    PacketHeader ack = congestion_control.GetAckPacket(last_process_time_ns, id,
                                                       packet.GetSourceID());
    packets_wrapped.push(
        RoutingPacket(ack, this->GetNextElement(packet.GetSourceID())));
}

ServerSender::ServerSender(std::uint64_t receiver_id, std::uint64_t id) {
    this->receiver_id = receiver_id;
    this->id = id;
}

std::uint64_t ServerSender::SendPackets(std::uint64_t current_time_ns,
                                        PriorityQueueWrapper &packets_wrapped) {
    if (ack_counter < max_ack) {
        last_process_time_ns =
            std::max(last_process_time_ns, current_time_ns) + process_time_ns;
        PacketHeader data = congestion_control.GetDataPacket(
            last_process_time_ns, id, receiver_id);
        ack_counter++;
        packets_wrapped.push(
            RoutingPacket(data, this->GetNextElement(data.GetDestinationID())));
        return last_process_time_ns;
    }
    return current_time_ns + 100;
}

void ServerSender::ReceivePacket(std::uint64_t current_time_ns,
                                 PacketHeader &packet,
                                 PriorityQueueWrapper &packets_wrapped) {
    if (packet.GetFlag(0) == 1) {
        max_ack++;
        ack_counter--;
    }
}

void ServerSender::DecreaseMaxAck() {
    max_ack /= 2;
}
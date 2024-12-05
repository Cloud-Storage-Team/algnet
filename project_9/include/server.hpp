#pragma once

#include "server_base.hpp"

class ServerReceiver : public ServerBase {
  private:
    std::vector<std::uint64_t> senders_ids;
    PacketGenerator congestion_control;

  public:
    explicit ServerReceiver(std::vector<std::uint64_t> senders_ids,
                            std::uint64_t id);

    std::uint64_t
    SendPackets(std::uint64_t current_time_ns,
                PriorityQueueWrapper &packets_wrapped) override final;
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader &packet,
                       PriorityQueueWrapper &packets_wrapped) override final;
};

class ServerSender : public ServerBase {
  private:
    std::uint64_t receiver_id;
    PacketGenerator congestion_control;
    std::uint64_t ack_counter = 0;
    std::uint64_t max_ack = 1;

  public:
    explicit ServerSender(std::uint64_t receiver_id, std::uint64_t id);

    std::uint64_t
    SendPackets(std::uint64_t current_time_ns,
                PriorityQueueWrapper &packets_wrapped) override final;
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader &packet,
                       PriorityQueueWrapper &packets_wrapped) override final;
    void DecreaseMaxAck();
};
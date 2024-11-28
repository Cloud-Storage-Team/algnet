#pragma once

#include "server.hpp"

class ExpressPassReciever : public ServerBase {
    // Add some fields for calculating statistics
  private:
    std::vector<std::uint64_t> senders_ids;
    std::unordered_map<std::uint64_t, std::uint64_t> ids_to_sending_time{};
    std::unordered_map<std::uint64_t, std::uint32_t>
        amount_of_data_from_sender{};
    std::uint64_t inter_credit_gap;
    ExpressPass congestion_control;

  public:
    explicit ExpressPassReciever(std::vector<std::uint64_t> senders_ids,
                                 std::uint64_t id,
                                 std::uint64_t inter_credit_gap);

    std::uint64_t
    SendPackets(std::uint64_t current_time_ns,
                PriorityQueueWrapper &packets_wrapped) override final;
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader &packet,
                       PriorityQueueWrapper &packets_wrapped) override final;
};

class ExpressPassSender : public ServerBase {
  private:
    // currently unused
    std::vector<PacketHeader> credit_bucket;
    std::uint8_t max_credit_bucket_size = 4;

    std::uint64_t process_time_ns;
    ExpressPass congestion_control;

  public:
    explicit ExpressPassSender(std::uint64_t id,
                               std::uint64_t process_time_ns = 10,
                               std::uint8_t max_credit_bucket_size = 4);

    std::uint64_t
    SendPackets(std::uint64_t current_time_ns,
                PriorityQueueWrapper &packets_wrapped) override final;
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader &packet,
                       PriorityQueueWrapper &packets_wrapped) override final;
};
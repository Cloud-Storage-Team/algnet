#pragma once
#include <string>
#include <sstream>

#include "server.hpp"
#include "express_pass.hpp"

struct IndexKeeper {
    std::uint32_t last_sent_index = 0;
    std::uint32_t last_received_index = 0;
};

class ExpressPassReceiver: public ServerBase {
private:
    std::vector<std::uint64_t> senders_ids;
    std::unordered_map<std::uint64_t, std::uint32_t> amount_of_data_from_sender{};
    std::unordered_map<std::uint64_t, IndexKeeper> flow_to_index{};
    std::unordered_map<std::uint64_t, std::uint32_t> flow_to_lost_credits{};
    std::uint64_t inter_credit_gap;
    std::uint64_t simulation_duration;
    std::uint64_t rtt = 1500;
    ExpressPass congestion_control;
public:
    explicit ExpressPassReceiver(std::vector<std::uint64_t> senders_ids, std::uint64_t id, std::uint64_t simulation_duration, std::uint64_t inter_credit_gap);

    std::uint64_t SendCreditByFlow(std::uint64_t flow_id, std::uint64_t current_time_ns, EventQueue& all_events);
    std::uint64_t SendPackets(std::uint64_t current_time_ns, EventQueue& all_events) override final;
    std::string ToString() const override final;
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, EventQueue& all_events) override final;
};

class ExpressPassSender: public ServerBase {
private:
    std::uint64_t receiver_id;
    std::vector<PacketHeader> credit_bucket;
    std::uint8_t max_credit_bucket_size = 4;

    std::uint64_t simulation_duration;
    std::uint64_t process_time_ns;
    ExpressPass congestion_control;
public:
    explicit ExpressPassSender(std::uint64_t receiver_id, std::uint64_t id, std::uint64_t simulation_duration, std::uint64_t process_time_ns = 10, std::uint8_t max_credit_bucket_size = 4);

    std::uint64_t SendPackets(std::uint64_t current_time_ns, EventQueue& all_events) override final;
    std::string ToString() const override final {
        return "";
    };
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, EventQueue& all_events) override final;
};
#pragma once

#include "server.hpp"

#include <string>
#include <sstream>

class ReceiverPacketNumber: public ServerBase {
private:
    std::uint32_t amount_of_data = 0;
    std::uint64_t simulation_duration;
public:
    ReceiverPacketNumber(std::uint64_t id, std::uint64_t simulation_duration): ServerBase(id), simulation_duration(simulation_duration) {};

    std::uint64_t SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) override final {
        return simulation_duration + 1;
    };

    std::string ToString() const override final {
        return std::to_string(amount_of_data);
    };

    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override final {
        std::cout << "Received: " << packet.GetSendingTime() << std::endl << std::endl;
        if (current_time_ns <= simulation_duration) {
            amount_of_data++;
        }
    };
};

class ReceiverPacketNumberPerSender: public ServerBase {
private:
    std::unordered_map<std::uint64_t, std::uint32_t> amount_of_data_from_sender{};
    std::uint64_t simulation_duration;
public:
    ReceiverPacketNumberPerSender() = default;
    ReceiverPacketNumberPerSender(std::uint64_t id, std::uint64_t simulation_duration): ServerBase(id), simulation_duration(simulation_duration) {};

    std::uint64_t SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) override final {
        return simulation_duration + 1;
    };

    std::string ToString() const override final {
        std::ostringstream oss;

        std::vector<std::pair<std::uint64_t, std::uint32_t>> result;
        for (auto& sender_and_data: amount_of_data_from_sender) {
            result.push_back(sender_and_data);
        }
        std::sort(result.begin(), result.end());

        for (auto& sender_and_data: result) {
            oss << "Id: "<< sender_and_data.first << ", Number: " << sender_and_data.second << "\n";
        }

        return oss.str();
    };
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override final {
        amount_of_data_from_sender[packet.GetSourceID()]++;
    };
};

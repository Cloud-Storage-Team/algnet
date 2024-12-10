#pragma once

#include "server.hpp"

class Sender: public ServerBase {
private:
    std::uint64_t receiver_id;
    std::uint64_t sending_time_offset;
    std::uint64_t sending_time_interval;
    std::uint64_t next_sending_time;
public:
    explicit Sender(std::uint64_t id, std::uint64_t receiver_id, std::uint64_t sending_time_offset = 0, std::uint64_t sending_time_interval = 1000):
        ServerBase(id),
        receiver_id(receiver_id),
        sending_time_offset(sending_time_offset),
        sending_time_interval(sending_time_interval) {
            next_sending_time = sending_time_offset;
        };

    std::uint64_t SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) override final {
        if (current_time_ns != next_sending_time) {
            return next_sending_time;
        }

        PacketHeader packet = PacketHeader(this->id, receiver_id, current_time_ns, 0, 8192);
        std::cout << "Sent: " << packet.sending_time << std::endl;
        packets_wrapped.push(RoutingPacket(packet, this->GetNextElement(packet.destination_id)));

        next_sending_time = current_time_ns + sending_time_interval;
        std::cout << "Next sending at: " << next_sending_time << std::endl << std::endl;
        return next_sending_time;
    };

    std::string ToString() const override final {
        return "";
    };
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override final {};
};

class SenderWithMultiplier: public ServerBase {
private:
    std::uint64_t receiver_id;
    std::uint64_t sending_time_interval;
    std::uint32_t multiplier;
    std::uint32_t packets_to_send = 1;
    std::uint64_t next_sending_time = 0;
public:
    explicit SenderWithMultiplier(std::uint64_t id, std::uint64_t receiver_id, std::uint32_t multiplier = 2, std::uint64_t sending_time_interval = 1000):
        ServerBase(id),
        receiver_id(receiver_id),
        multiplier(multiplier),
        sending_time_interval(sending_time_interval) {};

    std::uint64_t SendPackets(std::uint64_t current_time_ns, PriorityQueueWrapper& packets_wrapped) override final {
        if (current_time_ns != next_sending_time) {
            return next_sending_time;
        }

        for (int n = 0; n < packets_to_send; n++) {
            PacketHeader packet = PacketHeader(this->id, receiver_id, current_time_ns, n, 8192);
            packets_wrapped.push(RoutingPacket(packet, this->GetNextElement(packet.destination_id)));
        }
        packets_to_send *= multiplier;

        next_sending_time = current_time_ns + sending_time_interval;

        return next_sending_time;
    };
    std::string ToString() const override final {
        return "";
    };
    void ReceivePacket(std::uint64_t current_time_ns, PacketHeader& packet, PriorityQueueWrapper& packets_wrapped) override final;
};
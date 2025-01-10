#pragma once

#include <cstdint>

class Packet 
{
    public:
        explicit Packet(std::uint64_t src_id, std::uint64_t dst_id, std::uint64_t sending_time_ns, bool is_ack);
        Packet() = default;
        ~Packet() = default;

        // Getters
        std::uint64_t GetSourceID() const;
        std::uint64_t GetDestinationID() const;
        std::uint64_t GetSendingTime() const;
        std::uint32_t GetTransmittedDataSize() const;
        bool IsAck() const;
        void SetAck(bool value);
    private:
        std::uint64_t source_id_;
        std::uint64_t destination_id_;
        std::uint64_t sending_time_ns_;
        std::uint32_t data_size_bytes_;
        bool is_ack_;
};
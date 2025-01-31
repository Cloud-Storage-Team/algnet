#pragma once

#include "NetworkDevice.hpp"
#include "Packet.hpp"

class Sender: public NetworkDevice {
public:
    explicit Sender(std::uint64_t processing_delay_ns);
    ~Sender() override = default;

    void ProcessPacket(Packet p) override;
};

#pragma once

#include "NetworkDevice.hpp"
#include "Packet.hpp"

class Receiver: public NetworkDevice {
public:
    explicit Receiver(std::uint64_t process_time_ns);
    ~Receiver() override = default;

    void ProcessPacket(Packet p) override;
};
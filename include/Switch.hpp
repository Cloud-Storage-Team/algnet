#pragma once

#include "NetworkDevice.hpp"
#include "Packet.hpp"

class Switch: public NetworkDevice {
public:
    Switch(double processing_delay_ns);
    ~Switch() override = default;

    void ProcessPacket(Packet p) override;
};
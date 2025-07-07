#pragma once
#include <cstdint>
#include <string>

#include "logger/logger.hpp"
#include "packet/packet.hpp"
namespace test {

struct FakePacket : public sim::Packet {
    FakePacket(std::shared_ptr<sim::IRoutingDevice> device) {
        dest_id = device->get_id();
    };
};

}  // namespace test

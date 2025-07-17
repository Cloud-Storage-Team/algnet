#pragma once
#include <cstdint>
#include <string>

#include "packet.hpp"

#include "logger/logger.hpp"
namespace test {

struct FakePacket: public sim::Packet {
    FakePacket(std::shared_ptr<sim::IDevice> device) {
        dest_id = device->get_id();
    };
};

}  // namespace test

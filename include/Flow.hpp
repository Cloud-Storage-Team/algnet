#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>
#include <queue>

class Flow {
public:
    explicit Flow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver,
                  std::uint64_t packet_generation_interval_ns);

    void Send();

    std::shared_ptr<NetworkDevice> m_sender;
    std::shared_ptr<NetworkDevice> m_receiver;
    std::uint64_t m_packet_generation_interval_ns = 0;
};

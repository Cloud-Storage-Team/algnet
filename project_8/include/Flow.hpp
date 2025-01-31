#pragma once

#include "NetworkDevice.hpp"

#include <cstdint>
#include <memory>
#include <queue>

class Flow 
{   
    public:
        explicit Flow(std::shared_ptr<NetworkDevice> sender, std::shared_ptr<NetworkDevice> receiver,
                    std::uint64_t packet_generation_interval_ns);

        void Send();
    private:
        std::shared_ptr<NetworkDevice> sender_;
        std::shared_ptr<NetworkDevice> receiver_;
        std::uint64_t packet_generation_interval_ns_ = 0;
        std::uint32_t flow_id_;
        static std::uint32_t last_flow_id_;
};
std::uint32_t Flow::last_flow_id_ = 0;
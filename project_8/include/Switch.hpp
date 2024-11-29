#pragma once
#include <vector>
#include "NetworkNode.hpp"

namespace NetworkSimulator
{
    class Switch : public NetworkNode  
    {
        public:

        void send(NetworkSimulator::Packet packet) override
        {

        }
        private:

    };
}

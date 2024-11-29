#pragma once
#include <vector>
#include "NetworkNode.hpp"

namespace NetworkSimulator
{
    class Server : public NetworkNode  
    {
        public:

        void send(NetworkSimulator::Packet packet) override
        {
            
        }

        private:  

    };
}



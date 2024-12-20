#pragma once
#include <vector>
#include "NetworkDevice.hpp"
#include "Packet.hpp"

class Switch : public NetworkDevice  
{
    public:
        void ProcessPacket(Packet p) override;
        
    private:

};


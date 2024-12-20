#pragma once
#include "Switch.hpp"
#include "Packet.hpp"
void Switch::ProcessPacket(Packet p)
{
    Enqueue(p);
    
}
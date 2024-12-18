#include "NetworkDevice.hpp"

#include <memory>

void Flow::Send() {
    std::shared_ptr<NetworkDevice> sender = path.front();
    std::shared_ptr<NetworkDevice> receiver = path.back();

    Packet p(sender->id, receiver->id, 0, false);
    NetworkSimulator::Schedule(interval_ns, [&]() {
        sender->ProcessPacket(p);
        Send();
    });
}
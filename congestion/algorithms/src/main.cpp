#include "PowerTCPCongestionWindowHandler.h"

int main() {
    congestion::algorithms::PowerTCPCongestionWindowHandler cwnd_handler(10, 0.8, 0.5, 10);
    printf("CWND on start: %d\n", cwnd_handler.getCWND());
    std::vector<congestion::algorithms::INTHeader> headers;
    headers.push_back({5, 1, 12, 10});
    congestion::algorithms::Packet packet(0, headers);
    for (int i = 0; i < 10; ++i) {
        cwnd_handler.updateOnPacket(packet, 10);
        printf("Iteration %d: CWND after update on packet: %d\n", i, cwnd_handler.getCWND());
    }
    return 0;
}
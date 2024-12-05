#include "bfc_switch.hpp"

BFCSwitch::BFCSwitch(std::uint32_t numberOfQueues, std::uint32_t maxQueueSize,
                     std::uint32_t thresholdQueueSize)
    : numberOfQueues(numberOfQueues), maxQueueSize(maxQueueSize),
      thresholdQueueSize(thresholdQueueSize), queueVector(numberOfQueues),
      queuePauseCounter(numberOfQueues, 0), numberManager(numberOfQueues) {
}

int BFCSwitch::getQueue() {
    return numberManager.getNumber();
}

void BFCSwitch::returnQueue(int number) {
    numberManager.returnNumber(number);
}

void BFCSwitch::pause(int queueNumber) {
    queuePauseCounter[queueNumber]++;
}

void BFCSwitch::resume(int queueNumber) {
    queuePauseCounter[queueNumber]--;
}

void BFCSwitch::sendPause(PacketHeader &packet) {
    if (BFCSwitch *bfsSwitch = dynamic_cast<BFCSwitch *>(
            GetNextElement(packet.GetSourceID()).get())) {
        bfsSwitch->pause(packet.GetUpstreamQ());
    }
    if (ServerSender *sender = dynamic_cast<ServerSender *>(
            GetNextElement(packet.GetSourceID()).get())) {
        sender->DecreaseMaxAck();
    }
}

void BFCSwitch::sendResume(PacketHeader &packet) {
    if (BFCSwitch *bfsSwitch = dynamic_cast<BFCSwitch *>(
            GetNextElement(packet.GetSourceID()).get())) {
        bfsSwitch->resume(packet.GetUpstreamQ());
    }
}

std::uint64_t BFCSwitch::SendPackets(std::uint64_t current_time_ns,
                                     PriorityQueueWrapper &packets_wrapped) {

    for (int i = offset; (i + 1) % numberOfQueues != offset;
         i = (i + 1) % numberOfQueues) {
        if (!queueVector[i].empty() && queuePauseCounter[i] == 0) {
            offset = (i + 1) % numberOfQueues;
            PacketHeader packet = queueVector[i].front();
            queueVector[i].pop();

            std::size_t key =
                generateKey(packet.GetSourceID(),
                            packet.GetDestinationID());  // TODO +egress port?
            table[key].size -= 1;
            returnQueue(table[key].qAssignment);

            key = generateKey(packet.GetUpstreamQ());  // TODO +ingress port?

            if (packet.GetFlag(1)) {
                pauseCounter[key] -= 1;

                if (pauseCounter[key] == 0) {
                    // Resume the queue at upstream
                    sendResume(packet);  // also can work on
                                         // key with port
                }
            }
            last_process_time_ns =
                std::max(last_process_time_ns, current_time_ns) + process_time;
            packet.SetSendingTime(last_process_time_ns);
            packet.SetUpstreamQ(packet.GetQAssignment());
            packets_wrapped.push(RoutingPacket(
                packet, GetNextElement(packet.GetDestinationID())));
            return last_process_time_ns;
        }
    }
    return current_time_ns +
           100;  // as I understand it, the simulator requires time when a
                 // new package will be ready for transmission.  It's
                 // impossible to calculate, I'm returning the time, you can
                 // try to send it
}

void BFCSwitch::ReceivePacket(std::uint64_t current_time_ns,
                              PacketHeader &packet,
                              PriorityQueueWrapper &packets_wrapped) {
    if (packet.GetFlag(0) == 0) {
        packets_wrapped.push(
            RoutingPacket(packet, GetNextElement(packet.GetDestinationID())));
        return;
    }

    // std::cout << "Got: " << packet << std::endl;

    std::size_t key =
        generateKey(packet.GetSourceID(),
                    packet.GetDestinationID());  // TODO +egress Port?
    if (table[key].size >= maxQueueSize) {
        // std::cout << "Dropped: " << packet << std::endl;
        return;
    }

    bool reassignedQueue = false;

    if (table[key].size == 0) {
        reassignedQueue = true;
    }
    table[key].size += 1;

    if (reassignedQueue) {
        table[key].qAssignment = getQueue();
    }
    packet.SetQAssignment(table[key].qAssignment);

    key = generateKey(packet.GetUpstreamQ());  // TODO +ingress Port?

    if (queueVector[packet.GetQAssignment()].size() > thresholdQueueSize) {
        packet.SetFlag(2, 1);
        pauseCounter[key] += 1;

        if (pauseCounter[key] == 1) {
            // Pasue the queue at upstream
            sendPause(packet);  // also can work on key with port
        }
    }
    // std::cout << "Put into queue: " << packet << " " << packet.qAssignment
    //           << std::endl;
    queueVector[packet.GetQAssignment()].push(packet);
}

#include "BFCSwitch.hpp"
#include "NetworkSimulator.hpp"


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

void BFCSwitch::sendPause(Packet &packet) {

    if (packet.m_is_ack) {
        if (BFCSwitch *bfsSwitch = dynamic_cast<BFCSwitch *>(NextLink(packet.m_source_id)->destination.get())) {
                bfsSwitch->pause(packet.upstreamQ);
        }
    }
    else {
        if (BFCSwitch *bfsSwitch = dynamic_cast<BFCSwitch *>(NextLink(packet.m_destination_id)->destination.get())) {
                bfsSwitch->pause(packet.upstreamQ);
        }
    }
}

void BFCSwitch::sendResume(Packet &packet) {
    if (packet.m_is_ack) {
        if (BFCSwitch *bfsSwitch = dynamic_cast<BFCSwitch *>(NextLink(packet.m_source_id)->destination.get())) {
                bfsSwitch->resume(packet.upstreamQ);
        }
    }
    else {
        if (BFCSwitch *bfsSwitch = dynamic_cast<BFCSwitch *>(NextLink(packet.m_destination_id)->destination.get())) {
                bfsSwitch->resume(packet.upstreamQ);
        }
    }
}

void BFCSwitch::SelfProcessHandler(){

    for (int i = offset; (i + 1) % numberOfQueues != offset;
         i = (i + 1) % numberOfQueues) {
        if (!queueVector[i].empty() && queuePauseCounter[i] == 0) {
            offset = (i + 1) % numberOfQueues;
            Packet packet = queueVector[i].front();
            queueVector[i].pop();

            std::size_t key =
                generateKey(packet.m_source_id,
                            packet.m_destination_id);  // TODO +egress port?

            table[key].size -= 1;

            returnQueue(table[key].qAssignment);
            if (packet.upstreamQ!=-1) {
                key = generateKey(packet.upstreamQ);  // TODO +ingress port?
                
                if (packet.counterInc) {
                    pauseCounter[key] -= 1;

                    if (pauseCounter[key] == 0) {
                        // Resume the queue at upstream
                        sendResume(packet);  
                    }
                }
            }
            packet.upstreamQ=i;

            if (packet.m_is_ack) {
                /* send ACK to packet sender */
                NextLink(packet.m_source_id)->destination->ProcessPacket(packet);
            }
            else {
                /* send packet to receiver */
                NextLink(packet.m_destination_id)->destination->ProcessPacket(packet);
            }
            break;

        }
    }

    NetworkSimulator::Schedule(10, [this]() { this->SelfProcessHandler();});

}

void BFCSwitch::ProcessPacket(Packet packet) {


    std::size_t key =
        generateKey(packet.m_source_id,
                    packet.m_destination_id);  // TODO +egress Port?
    if (table[key].size >= maxQueueSize) {
        // std::cout << "Dropped " << std::endl;
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

    if (packet.upstreamQ!=-1){

    }
    std::size_t key2 = generateKey(packet.upstreamQ);  // TODO +ingress Port?

    if (queueVector[table[key].qAssignment].size() > thresholdQueueSize) {
        packet.counterInc = true;
        pauseCounter[key2] += 1;

        if (pauseCounter[key2] == 1) {
            // Pasue the queue at upstream
            sendPause(packet);
        }
    }

    queueVector[table[key].qAssignment].push(packet);


}

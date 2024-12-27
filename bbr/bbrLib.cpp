#include "bbrLib.hpp"
#include <math.h>
#include <cstdint>
/*
class Packet {
public:
    explicit Packet(std::uint64_t sending_time_ns);
    Packet() = default;
    ~Packet() = default;

    /**
     * @brief Packet sending time in nanoseconds
     
    std::uint64_t sending_time_ns;

    /**
     * @brief Number of successfully delivered bytes
     
    std::uint32_t transmitted_data_size_bytes;
};

Packet::Packet(std::uint64_t sending_time_ns):
        sending_time_ns(sending_time_ns) { }


*/




void bbr::main(Packet pasket){
    this->lastRtt = pasket.sending_time_ns;
    this->lstDelivered = pasket.transmitted_data_size_bytes;

}

void bbr::updateBw(){
    this->lastBw = this->lstDelivered / this->lastRtt;
    this->bwNow = std::max(this->bwNow,this->lastBw);
}

void bbr::updateProbePhase(){
    if(this->state != stage::BBR_PROBE_BW){
        return;
    }
    this->phase ++;
    this->phase %= this->cyclePhases;
}

void bbr::checkIsBwFull(){
    int bwTrash = this->bwMax * 1.25;
    if(this->bwNow >= bwTrash){
        this->bwMax = this->bwNow;
        this->fullRoinds = 0;
        return;
    }   

	this->fullRoinds++;
	this->fullBwReached = this->fullRoinds >= 3;//bbr_full_bw_cnt
}

void bbr::checkDrain(){
    if(this->state == stage::BBR_STARTUP && this->fullBwReached){
        this->state = stage::BBR_DRAIN;
    }
}

void bbr::updateState(){
    this->updateBw();
    this->updateProbePhase();
    this->checkIsBwFull();
    this->checkDrain();
}

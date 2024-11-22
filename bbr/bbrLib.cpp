#include "bbrLib.hpp"
#include <math.h>


int bbr::getCountPackage(int lastbw, int lastRtt){
    minRtt = std::min(minRtt, lastRtt);
    this->lastRtt = lastRtt;
    bwEstimate = lastbw;
    this->stateMachine();
    return cwnd * cwndCoefficient;
};

void bbr::stateMachine(){
    switch (this->state){
        case BBR_STARTUP:
            this->startup();
            break;
        case BBR_DRAIN:
            this->drain();
            break;
        case BBR_NORMAL:
            this->normal();
            break;
        case BBR_PROBE_BW:
            this->probeBW();
            break;
        case BBR_PROBE_BW_CHECK:
            this->probeBWCheck();
            break;
        case BBR_PROBE_RTT:
            this->probeRtt();
            break;
        default:
            break;
    }
};

void bbr::startup(){
    if(cwnd < maxcwnd){
        if(cwnd > bwEstimate * minRtt || minRtt * 1.05 < lastRtt){
            state = BBR_DRAIN;
        }else{
            cwnd *= 2;
        }
    }
    if(cwnd >= maxcwnd){
        cwnd = maxcwnd;
        state = BBR_DRAIN;
    }
};

void bbr::drain(){
    cwnd = std::max(cwnd / 2.0, bwEstimate * minRtt);
    cwndCoefficient = 0.5;
    state = BBR_NORMAL;
};

void bbr::normal(){
    tick ++;
    tick %= 10;
    cwndCoefficient = 1;
    if(tick == 0){
        state = BBR_PROBE_BW;
    }
}

void bbr::probeBW(){
    lastCwnd = cwnd;
    cwnd *= 1.02;
    state = BBR_PROBE_BW_CHECK;
    
};

void bbr::probeBWCheck(){
    if(cwnd < maxcwnd){
        if(cwnd > bwEstimate * minRtt || minRtt * 1.05 < lastRtt){
            state = BBR_NORMAL;
            cwnd = lastCwnd;
        }else{    
            state = BBR_PROBE_BW;
        }
    }
};

void bbr::probeRtt(){
    cwndCoefficient = 0.5;
    state = BBR_NORMAL;
}

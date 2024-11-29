#include "bbrLib.hpp"
#include <math.h>


bbr::bbr(double downPackageCoefficient, double upPackageCoefficient, int maxcwnd):
    downPackageCoefficient(downPackageCoefficient), 
    upPackageCoefficient(upPackageCoefficient),
    maxcwnd(maxcwnd){
    
}

int bbr::getCountPackage(int lastbw, int lastRtt){
    minRtt = std::min(minRtt, lastRtt);
    this->lastRtt = lastRtt;
    bwEstimate = lastbw;
    this->stateMachine();
    return cwnd * cwndCoefficient;
};

void bbr::stateMachine(){
    switch (this->state){
        case stage::BBR_STARTUP:
            this->startup();
            break;
        case stage::BBR_DRAIN:
            this->drain();
            break;
        case stage::BBR_NORMAL:
            this->normal();
            break;
        case stage::BBR_CHECK_DOWN_SPEED:
            this->checkDownSpeed();
            break;
        case stage::BBR_PROBE_BW:
            this->probeBW();
            break;
        case stage::BBR_PROBE_BW_CHECK:
            this->probeBWCheck();
            break;
        case stage::BBR_PROBE_RTT:
            this->probeRtt();
            break;
        default:
            break;
    }
};

void bbr::startup(){
    if(cwnd < maxcwnd){
        if(cwnd > bwEstimate * minRtt || minRtt * 1.05 < lastRtt){
            state = stage::BBR_DRAIN;
            return;
        }
        cwnd *= 2;
        
    }
    if(cwnd >= maxcwnd){
        cwnd = maxcwnd;
        state = stage::BBR_DRAIN;
    }
};

void bbr::drain(){
    cwnd = std::max(cwnd / 2.0, bwEstimate * minRtt);
    cwndCoefficient = 0.5;
    state = stage::BBR_NORMAL;
};

void bbr::normal(){
    tick ++;
    tick %= 10;
    cwndCoefficient = 1;
    if(tick == 0){
        state = stage::BBR_CHECK_DOWN_SPEED;
    }
}

void bbr::checkDownSpeed(){
    if(cwnd < maxcwnd){
        if(cwnd > bwEstimate * minRtt || minRtt * 1.05 < lastRtt){
            state = stage::BBR_CHECK_DOWN_SPEED;
            cwnd = cwnd * this->downPackageCoefficient;
        }else{
            state = stage::BBR_PROBE_BW_CHECK;
        }
    }
};

void bbr::probeBW(){
    lastCwnd = cwnd;
    cwnd *= this->upPackageCoefficient;
    state = stage::BBR_PROBE_BW_CHECK;
    
};

void bbr::probeBWCheck(){
    if(cwnd < maxcwnd){
        if(cwnd > bwEstimate * minRtt || minRtt * 1.05 < lastRtt){
            state = stage::BBR_NORMAL;
            cwnd = lastCwnd;
        }else{    
            state = stage::BBR_PROBE_BW;
        }
    }
};

void bbr::probeRtt(){
    cwndCoefficient = 0.5;
    state = stage::BBR_NORMAL;
}

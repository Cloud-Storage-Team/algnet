#pragma once
#include <chrono>
#include <algorithm>

const int BBR_STARTUP = 0;
const int BBR_DRAIN = 1;
const int BBR_NORMAL = 2;
const int BBR_PROBE_BW = 3;
const int BBR_CHECK_DOWN_SPEED = 4;
const int BBR_PROBE_BW_CHECK = 5;
const int BBR_PROBE_RTT = 6;



class bbr{
private:
    int state = 0; 
    double bwEstimate;         
    double bw_max; 
    double cwndCoefficient = 1;
    int lastRtt;
    int minRtt = 10000000; 
    int tick = 1;
    int updateTick;  
    int cwnd = 1;
    int maxcwnd;
    int lastCwnd;
public:
    bbr(/* args */);
    ~bbr();

    int getCountPackage(int uId, int lastRtt);

    void stateMachine();

    void startup();
    
    void drain();

    void normal();
    
    void checkDownSpeed();

    void probeBW();
    
    void probeRtt();

    void probeBWCheck();
};


#pragma once
#include <chrono>
#include <algorithm>


enum class stage{
    BBR_STARTUP,// to BBR_DRAIN
    BBR_DRAIN,  // to BBR_PROBE_BW
    BBR_PROBE_BW,//to BBR_PROBE_BW or BBR_PROBE_RTT
    BBR_PROBE_RTT//to BBR_PROBE_BW or BBR_STARTUP
};// We always can go to drain if RTT to big WS




class bbr{
private:
    stage state = stage::BBR_STARTUP; 

    int bwMax; 
    int bwNow; 
    int lastRtt;
    int lastBw;

    int lstDelivered;
    int minRtt = 10000000; 
    int tick = 1;

    int phase = 1; 
    int cyclePhases;

    int cwnd = 1;
    int maxcwnd;
    int lastCwnd;
    int fullRoinds = 0;
    bool fullBwReached = false;
public:
    bbr(int cyclePhases);
    ~bbr();
    void main(Packet packet);
    int getCountPackage(int uId, int lastRtt);
private:
    void checkIsBwFull();
    
    void updateState();

    void updateProbePhase();

    void updateBw();

    void startup();
    
    void drain();

    void normal();
    
    void checkDownSpeed();

    void probeBW();
    
    void probeRtt();

    void probeBWCheck();

    void checkDrain();

    void main();
};


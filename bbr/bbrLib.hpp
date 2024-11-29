#pragma once
#include <chrono>
#include <algorithm>


enum class stage{
    BBR_STARTUP,
    BBR_DRAIN,
    BBR_NORMAL, 
    BBR_PROBE_BW,
    BBR_CHECK_DOWN_SPEED,
    BBR_PROBE_BW_CHECK,
    BBR_PROBE_RTT
};




class bbr{
private:
    stage state = stage::BBR_STARTUP; 
    double bwEstimate;         
    double bwMax; 
    double cwndCoefficient = 1;
    double downPackageCoefficient;
    double upPackageCoefficient;
    int lastRtt;
    int minRtt = 10000000; 
    int tick = 1;
    int updateTick;  
    int cwnd = 1;
    int maxcwnd;
    int lastCwnd;
public:
    bbr(double downPackageCoefficient, double upPackageCoefficient, int maxcwnd);
    ~bbr();

    int getCountPackage(int uId, int lastRtt);
private:
    void stateMachine();

    void startup();
    
    void drain();

    void normal();
    
    void checkDownSpeed();

    void probeBW();
    
    void probeRtt();

    void probeBWCheck();
};


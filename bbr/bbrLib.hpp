#pragma once
#include <chrono>
#include <algorithm>

const int BBR_STARTUP = 0;
const int BBR_DRAIN = 1;
const int BBR_NORMAL = 2;
const int BBR_PROBE_BW = 3;
const int BBR_PROBE_BW_CHECK = 4;
const int BBR_PROBE_RTT = 5;
struct userData
{
    //last_rtt
    //minRtt
    //...
};


class bbr{
private:
    int state = 0;  // Текущее состояние BBR
    double bwEstimate;         // Оценка пропускной способности (BtlBw)
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
    /*
    Работает с увеличением по экспоненте пока не дойдёт до ограничения пропускного канала.
    */
    void drain();

    void normal();

    void upPacketInDrain();
    /*
    Очищение всех очередей на сервере.
    */
    void probeBW();
    
    /*
    измеряем текущую пропускную способность
    Состояний нужно больше тк сначала увеличиваем количество пакетов потом уменьшаем.
    */

    /*
    проверяем изменения rtt паралельно очищая очередь.
    Добавить больше состояний чем было
    */
   void probeRtt();

   void probeBWCheck();
};


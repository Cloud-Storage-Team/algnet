#pragma once
#include <chrono>
#include <algorithm>

const int BBR_STARTUP = 0;
const int BBR_DRAIN = 1;
const int BBR_NORMAL = 2;
const int BBR_PROBE_BW = 3;
const int BBR_CHECK = 4;
const int BBR_PROBE_RTT = 5;
struct userData
{
    //last_rtt
    //min_rtt
    //...
};

/*
TODO:
Разделить всё на два файла.
Написать make файл???
подумать над хранилищем.
Какие данные нужны от тестовой системы

*/
class bbr
{
private:
    int state = 0;  // Текущее состояние BBR
    double bw_estimate;         // Оценка пропускной способности (BtlBw)
    int min_rtt;       // Минимальная задержка (RTprop)
    int last_rtt;      // Последняя измеренная RTT      
    int min_rtt; 
    int tick = 1;
    int updateTick;  
    int packetCount = 1;
    int maxPacketCount;
public:
    bbr(/* args */);
    ~bbr();
    int getCountPackage(int uId, int lastRtt){
        this->stateMachine();
        return packetCount;
    };
    void stateMachine(){
        switch (this->state)
        {
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
        case BBR_PROBE_RTT:
            this->probeRtt();
            break;
        
        default:
            break;
        }
    };
    void startup(){
        // Добавить ограничение в случае падения времени доставки пакета
        if(packetCount < maxPacketCount){
            packetCount *= 2;
        }
        if(packetCount >= maxPacketCount){
            packetCount = maxPacketCount;
            state = BBR_DRAIN;
        }
    };
    /*
    Работает с увеличением по экспоненте пока не дойдёт до ограничения пропускного канала.
    */
    void drain(){
        packetCount = packetCount / 2;
        state = BBR_NORMAL;
    };

    void normal(){
        packetCount = maxPacketCount;
        tick++;
        tick%=10;
        if(tick == 0){
            state = BBR_PROBE_BW;
        }
    }

    void upPacketInDrain();
    /*
    Очищение всех очередей на сервере.
    */
    void probeBW(){
        //if(){
        if(packetCount / 100 == 0){
            packetCount += 1;
        }else{
            packetCount += (packetCount / 100);
        }
        //}else{
        //state = BBR_DRAIN
        //}
    };
    
    /*
    измеряем текущую пропускную способность
    Состояний нужно больше тк сначала увеличиваем количество пакетов потом уменьшаем.
    */

    /*
    проверяем изменения rtt паралельно очищая очередь.
    Добавить больше состояний чем было
    */
   void probeRtt(){
        packetCount = packetCount / 2;
        state = BBR_NORMAL;
   }
};


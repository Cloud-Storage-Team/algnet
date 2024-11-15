#include "bbrLib.hpp"
/*
TODO:
Разделить всё на два файла.
Написать make файл???
подумать над хранилищем.
Какие данные нужны от тестовой системы?

*/

int bbr::getCountPackage(int uId, int lastRtt){
    this->stateMachine();
    return packetCount;
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
        case BBR_PROBE_RTT:
            this->probeRtt();
            break;
        default:
            break;
    }
};

void bbr::startup(){
    // Добавить ограничение в случае падения времени доставки пакета
    if(packetCount < maxPacketCount){
        packetCount *= 2;
    }
    if(packetCount >= maxPacketCount){
        packetCount = maxPacketCount;
        state = BBR_DRAIN;
    }
};
void bbr::drain(){
    packetCount = packetCount / 2;
    state = BBR_NORMAL;
};

void bbr::normal(){
    packetCount = maxPacketCount;
    tick++;
    tick%=10;
    if(tick == 0){
        state = BBR_PROBE_BW;
    }
}

void bbr::probeBW(){
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

void bbr::probeRtt(){
        packetCount = packetCount / 2;
        state = BBR_NORMAL;
   }

#include "number_manager.hpp"

NumberManager::NumberManager(int count) {
    for (int i = 0; i < count; ++i) {
        numbers.insert(NumberInfo(i)); 
    }
}

int NumberManager::getNumber() {
    NumberInfo numberInfo = *numbers.begin();
    numbers.erase(numberInfo);
    numbers.insert(NumberInfo(numberInfo.number, numberInfo.usageCount + 1));
    return numberInfo.number;
}

void NumberManager::returnNumber(int number) {
    for (auto it = numbers.begin(); it != numbers.end(); ++it) {
        if (it->number == number) {
            NumberInfo numberInfo = *it; 
            numbers.erase(it);
            numbers.insert(NumberInfo(numberInfo.number, numberInfo.usageCount - 1));
            break; 
        }
    }
}
#pragma once

#include <set>

class QNumberManager {
  private:
    struct NumberInfo {
        int number;      // The actual number
        int usageCount;  // The count of how many times this number has
                         // been used
        NumberInfo(int n, int c = 0) : number(n), usageCount(c) {
        }

        bool operator<(const NumberInfo &other) const {
            if (usageCount == other.usageCount) {
                return number < other.number;
            }
            return usageCount < other.usageCount;
        }
    };

    std::set<NumberInfo> numbers;

  public:
    /**
     * @brief Constructs a NumberManager with a specified range of numbers.
     *
     * @param count The total number of unique numbers to manage, ranging
     * from 0 to count-1.
     */
    QNumberManager(int count);
    /**
     * @brief Provides a number from the available pool with lowes number of
     * usages, incrementing its usage count.
     *
     * @return int The number from the pool.
     */
    int getNumber();

    /**
     * @brief Returns a previously used number back to the pool,
     * decrementing its usage count.
     *
     * @param number The number to be returned to the pool.
     */
    void returnNumber(int number);
};
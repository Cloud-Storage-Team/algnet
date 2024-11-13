#include "TransmissionUnit.hpp"

TransmissionUnit::TransmissionUnit(std::uint64_t delivery_time_us)
    : estimated_delivery_time(delivery_time_us) { }

std::uint64_t TransmissionUnit::GetEstimatedDeliveryTime() const {
    return estimated_delivery_time;
}

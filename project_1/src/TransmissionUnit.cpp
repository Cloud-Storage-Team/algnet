#include "TransmissionUnit.hpp"

TransmissionUnit::TransmissionUnit(std::uint64_t delivery_time_ns)
    : estimated_delivery_time(delivery_time_ns) { }

std::uint64_t TransmissionUnit::GetEstimatedDeliveryTime() const {
    return estimated_delivery_time;
}

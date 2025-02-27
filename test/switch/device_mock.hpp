#pragma once
#include "../../source/device.hpp"
#include "../../source/link.hpp"

class DeviceMock : public sim::Device {
public:
    DeviceMock(sim::DeviceType a_type);
    ~DeviceMock() = default;

    void process() override;
};

#include "device_mock.hpp"

DeviceMock::DeviceMock(sim::DeviceType a_type) : sim::Device(a_type) {}

void DeviceMock::process() {}

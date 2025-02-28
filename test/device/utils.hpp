#pragma once
#include <gtest/gtest.h>

#include "device.hpp"
#include "link.hpp"

namespace sim {

class Link;

class TestDevice : public Device {
public:
    TestDevice();
    ~TestDevice() = default;

    void process();

    bool has_inlink(Link* link);
    bool check_rout(Device* device, Link* link);
};

class TestLink : public sim::Link {
public:
    TestLink(sim::Device* a_src, sim::Device* a_dest);
    ~TestLink() = default;
};

}  // namespace sim

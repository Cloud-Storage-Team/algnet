#pragma once
#include <gtest/gtest.h>

#include "../../source/device.hpp"
#include "../../source/link.hpp"

namespace test {

class Link;

class TestDevice : public sim::Device {
public:
    TestDevice();
    ~TestDevice() = default;

    void process() final;

    bool has_inlink(sim::Link* link);
    bool check_route(sim::Device* device, sim::Link* link);
};

class TestLink : public sim::Link {
public:
    TestLink(sim::Device* a_src, sim::Device* a_dest);
    ~TestLink() = default;
};

}  // namespace test

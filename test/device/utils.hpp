#pragma once
#include <gtest/gtest.h>

#include "../../source/device.hpp"
#include "../../source/link.hpp"

namespace test {

class Link;

class TestRoutingModule : public sim::RoutingModule {
public:
    TestRoutingModule();
    ~TestRoutingModule() = default;

    bool has_inlink(std::shared_ptr<sim::Link> link);
    bool check_route(std::shared_ptr<sim::IRoutingDevice> device, std::shared_ptr<sim::Link> link);
};

class TestLink : public sim::Link {
public:
    TestLink(std::shared_ptr<sim::IRoutingDevice> a_src, std::shared_ptr<sim::IRoutingDevice> a_dest);
    ~TestLink() = default;
};

}  // namespace test

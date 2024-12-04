#include <gtest/gtest.h>

#include "PowerTCPCongestionWindowHandler.h"

TEST(PowerTCPCongestionWindowHandler, DummyTest) {
    congestion::algorithms::PowerTCPCongestionWindowHandler handler(10, 0.8, 10, 10);
    EXPECT_EQ(10, handler.getCWND());
    EXPECT_EQ(1, handler.getRate());

    {
        std::vector<congestion::algorithms::INTHeader> headers;
        headers.push_back({0, 0, 0, 1});
        headers.push_back({5, 1, 0, 1});
        congestion::algorithms::Packet packet{0, headers};
        handler.updateOnPacket(packet, 10);
        EXPECT_EQ(82, handler.getCWND());
        EXPECT_EQ(8, handler.getRate());
    }

    {
        std::vector<congestion::algorithms::INTHeader> headers;
        headers.push_back({10, 0, 16, 1});
        headers.push_back({15, 1, 16, 1});
        congestion::algorithms::Packet packet{10, headers};
        handler.updateOnPacket(packet, 20);
        EXPECT_EQ(61, handler.getCWND());
        EXPECT_EQ(6, handler.getRate());
    }
}

TEST(PowerTCPCongestionWindowHandler, CongestionTest) {
    congestion::algorithms::PowerTCPCongestionWindowHandler handler(10, 0.8, 10, 10);
    EXPECT_EQ(10, handler.getCWND());
    EXPECT_EQ(1, handler.getRate());

    {
        std::vector<congestion::algorithms::INTHeader> headers;
        headers.push_back({0, 0, 0, 1});
        headers.push_back({5, 1, 0, 1});
        congestion::algorithms::Packet packet{0, headers};
        handler.updateOnPacket(packet, 10);
        EXPECT_EQ(82, handler.getCWND());
        EXPECT_EQ(8, handler.getRate());
    }

    {
        std::vector<congestion::algorithms::INTHeader> headers;
        headers.push_back({10, 0, 16, 1});
        headers.push_back({15, 4, 16, 1});
        congestion::algorithms::Packet packet{10, headers};
        handler.updateOnPacket(packet, 20);
        EXPECT_EQ(49, handler.getCWND());
        EXPECT_EQ(4, handler.getRate());
    }

    {
        std::vector<congestion::algorithms::INTHeader> headers;
        headers.push_back({20, 0, 32, 1});
        headers.push_back({25, 8, 32, 1});
        congestion::algorithms::Packet packet{20, headers};
        handler.updateOnPacket(packet, 30);
        EXPECT_EQ(28, handler.getCWND());
        EXPECT_EQ(2, handler.getRate());
    }

    {
        std::vector<congestion::algorithms::INTHeader> headers;
        headers.push_back({30, 0, 48, 1});
        headers.push_back({35, 1, 160, 1});
        congestion::algorithms::Packet packet{30, headers};
        handler.updateOnPacket(packet, 40);
        EXPECT_EQ(13, handler.getCWND());
        EXPECT_EQ(1, handler.getRate());
    }

    {
        std::vector<congestion::algorithms::INTHeader> headers;
        headers.push_back({40, 0, 64, 1});
        headers.push_back({45, 1, 176, 1});
        congestion::algorithms::Packet packet{40, headers};
        handler.updateOnPacket(packet, 50);
        EXPECT_EQ(16, handler.getCWND());
        EXPECT_EQ(1, handler.getRate());
    }
}
#include "Server.hpp"
#include "Packet.hpp"
#include "Flow.hpp"

#include <gtest/gtest.h>


/** ServerBase and ServerSender tests */
TEST(ServerTest, ServerBaseGettersTest) {
    ServerBase server(0);

    EXPECT_EQ(server.GetID(), 0);
    EXPECT_EQ(server.GetCurrentTime(), 0);
}

TEST(ServerTest, ServerSenderGettersTest) {
    std::uint32_t distance_ns = 500;
    std::uint32_t id = 1;
    ServerSender server(id, distance_ns);

    EXPECT_EQ(server.GetID(), id);
    EXPECT_EQ(server.GetCurrentTime(), 0);
    EXPECT_EQ(server.GetCWNDSize(), 1);
    EXPECT_EQ(server.GetDistance(), distance_ns);
}

TEST(ServerTest, CurrentTimeTest) {
    ServerBase server(0);
    server.IncreaseCurrentTime(150);
    EXPECT_EQ(server.GetCurrentTime(), 150);

    server.IncreaseCurrentTime(0);
    EXPECT_EQ(server.GetCurrentTime(), 150);

    server.IncreaseCurrentTime(1000);
    EXPECT_EQ(server.GetCurrentTime(), 1150);

    server.SetCurrentTime(2000);
    EXPECT_EQ(server.GetCurrentTime(), 2000);
}

/** Packet tests */
TEST(PacketTest, PacketGettersTest) {
    Packet packet(1, 100, 600);
    EXPECT_EQ(packet.GetSenderId(), 1);
    EXPECT_EQ(packet.GetEstimatedDeliveryTime(), 600);
}

TEST(PacketTest, PacketOrderTest) {
    Packet fst_packet(1, 100, 600);
    Packet snd_packet(2, 300, 550);

    EXPECT_EQ(fst_packet < snd_packet, true);
}

/** Flow tests */
TEST(FlowTest, NodesTest) {
    std::vector<std::uint32_t> distances_ns = {500, 1000, 5000};
    Flow flow(distances_ns);

    EXPECT_EQ(flow.GetDestinationNode().GetID(), 0);
    EXPECT_EQ(flow.GetDestinationNode().GetCurrentTime(), 0);

    EXPECT_EQ(flow.GetSourceNodes().size(), 3);
    for (std::uint32_t i = 0; i < distances_ns.size(); ++i) {
        EXPECT_EQ(flow.GetSourceNodes()[i].GetDistance(), distances_ns[i]);
        EXPECT_EQ(flow.GetSourceNodes()[i].GetCurrentTime(), 0);
        EXPECT_EQ(flow.GetSourceNodes()[i].GetCWNDSize(), 1);
    }
}
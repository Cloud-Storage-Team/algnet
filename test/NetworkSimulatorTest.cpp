#include "Event.hpp"
#include "EventScheduler.hpp"
#include "Link.hpp"
#include "Sender.hpp"
#include "Switch.hpp"
#include "NetworkSimulator.hpp"

#include <gtest/gtest.h>
#include <functional>
#include <memory>

TEST(EventTest, EventOrderTest) {
    std::uint64_t event_time = 1000;
    std::function<void()> empty_handler = [](){};

    Event first_event(event_time, empty_handler);
    Event second_event(event_time, empty_handler);
    Event third_event(event_time + 10, empty_handler);

    ASSERT_TRUE(third_event < first_event);

    // First and second events are equal
    ASSERT_FALSE(first_event < second_event);
    ASSERT_FALSE(second_event < first_event);
}

TEST(EventTest, EventHandlerTest) {
    std::uint64_t event_time = 1000;
    int test_value = 0;
    std::function<void()> handler = [&test_value](){
        test_value += 100;
    };
    Event first_event(event_time, handler);
    Event second_event(event_time, handler);

    ASSERT_TRUE(test_value == 0);
    first_event.Execute();
    ASSERT_TRUE(test_value == 100);
    second_event.Execute();
    ASSERT_TRUE(test_value == 200);
}

TEST(EventSchedulerTest, EventSchedulerPriorityQueueTest) {
    EventScheduler es;

    std::uint64_t first_event_time = 1000;
    std::uint64_t second_event_time = 1010;
    std::function<void()> empty_handler = [](){};

    ASSERT_TRUE(es.Empty());

    es.Schedule(first_event_time, empty_handler);

    ASSERT_TRUE(!es.Empty());
    ASSERT_TRUE(es.PeekNextEventTime() == first_event_time);

    es.Schedule(second_event_time, empty_handler);

    ASSERT_TRUE(!es.Empty());
    ASSERT_TRUE(es.events.size() == 2);
    ASSERT_TRUE(es.PeekNextEventTime() == first_event_time);

    Event e = es.PopNextEvent();

    ASSERT_TRUE(e.execution_time == first_event_time);
    ASSERT_TRUE(!es.Empty());
    ASSERT_TRUE(es.PeekNextEventTime() == second_event_time);
    ASSERT_TRUE(es.events.size() == 1);

    e = es.PopNextEvent();

    ASSERT_TRUE(e.execution_time == second_event_time);
    ASSERT_TRUE(es.Empty());
}

TEST(LinkTest, LinkLastProcessTimeTest) {
    std::shared_ptr<NetworkDevice> src = std::make_shared<Sender>(100);
    std::shared_ptr<NetworkDevice> dst = std::make_shared<Switch>(100);
    std::uint64_t distance_ns = 1000;
    Link link(src, dst, distance_ns);

    ASSERT_TRUE(link.last_processing_time_ns == 0);

    link.UpdateLastProcessTime(50);
    ASSERT_TRUE(link.last_processing_time_ns == 50);

    link.UpdateLastProcessTime(1000);
    ASSERT_TRUE(link.last_processing_time_ns == 1000);

    link.UpdateLastProcessTime(0);
    ASSERT_TRUE(link.last_processing_time_ns == 1000);
}

TEST(NetworkSimulatorTest, NetworkSimulatorBasicTest) {
    NetworkSimulator ns;
    std::shared_ptr<Sender> sender = std::make_shared<Sender>(100);
    std::shared_ptr<Switch> swtch = std::make_shared<Switch>(100);
    std::shared_ptr<Link> link1 = std::make_shared<Link>(sender, swtch, 100);
    std::shared_ptr<Flow> flow = std::make_shared<Flow>(sender, swtch, 100);
    NetworkSimulator::AddDevice(swtch);
    NetworkSimulator::AddDevice(sender);
    NetworkSimulator::AddLink(link1, flow);
    NetworkSimulator::AddFlow(flow);

    ASSERT_TRUE(NetworkSimulator::Now() == 0);
    ASSERT_TRUE(sender->routing_table.find(swtch->id) != sender->routing_table.end());
    ASSERT_TRUE(swtch->routing_table.find(sender->id) != swtch->routing_table.end());
    ASSERT_TRUE(NetworkSimulator::device_by_id[sender->id] == sender);
    ASSERT_TRUE(NetworkSimulator::device_by_id[swtch->id] == swtch);
    ASSERT_TRUE(NetworkSimulator::flows.size() == 1);
    ASSERT_TRUE(NetworkSimulator::flows[0]->getSenderID() == sender->id);
    ASSERT_TRUE(NetworkSimulator::flows[0]->getReceiverID() == swtch->id);
}

TEST(NetworkDeviceTest, RoutingTableTest) {
    NetworkSimulator ns;
    std::shared_ptr<Sender> sender = std::make_shared<Sender>(100);
    std::shared_ptr<Switch> swtch = std::make_shared<Switch>(100);
    std::shared_ptr<Receiver> receiver = std::make_shared<Receiver>(100);
    std::shared_ptr<Link> link1 = std::make_shared<Link>(sender, swtch, 100);
    std::shared_ptr<Link> link2 = std::make_shared<Link>(swtch, receiver, 100);
    std::shared_ptr<Flow> flow = std::make_shared<Flow>(sender, receiver, 100);
    NetworkSimulator::AddDevice(swtch);
    NetworkSimulator::AddDevice(sender);
    NetworkSimulator::AddDevice(receiver);
    NetworkSimulator::AddLink(link1, flow);
    NetworkSimulator::AddLink(link2, flow);
    NetworkSimulator::AddFlow(flow);


    ASSERT_TRUE(sender->routing_table.size() == 1); // link to switch
    ASSERT_TRUE(swtch->routing_table.size() == 2);  // link to sender and link to receiver
    ASSERT_TRUE(receiver->routing_table.size() == 1); // link to switch

    ASSERT_TRUE(sender->routing_table.find(receiver->id) != sender->routing_table.end());
    ASSERT_TRUE(swtch->routing_table.find(sender->id) != swtch->routing_table.end());
    ASSERT_TRUE(swtch->routing_table.find(receiver->id) != swtch->routing_table.end());
    ASSERT_TRUE(receiver->routing_table.find(sender->id) != receiver->routing_table.end());
}
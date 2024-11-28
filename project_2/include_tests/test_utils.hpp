#include "simulator.hpp"
#include "test_receivers.hpp"
#include "test_senders.hpp"
#include "test_switches.hpp"

#include <functional>

template <typename ReceiverType, typename SenderType, typename... SenderArgs>
std::string RunSimulation(
    std::uint32_t amount_of_senders,
    std::uint64_t simulation_duration,
    std::uint64_t switch_buffer_size,
    std::uint64_t switch_process_time,
    std::function<std::shared_ptr<SenderType>(std::uint64_t, std::uint64_t, int, SenderArgs...)> senderFactory,
    SenderArgs... senderArgs)
{
    static_assert(std::is_base_of<ServerBase, SenderType>::value, "SenderType must derive from ServerBase");
    static_assert(std::is_base_of<ServerBase, ReceiverType>::value, "ReceiverType must derive from ServerBase");

    std::vector<std::shared_ptr<ServerBase>> senders;
    std::vector<std::uint64_t> senders_ids;
    
    std::uint64_t receiver_id = NetworkSimulator::GenerateNewID();
    std::shared_ptr<ServerBase> receiver(new ReceiverType(receiver_id, simulation_duration));

    for (int sender_index = 0; sender_index < amount_of_senders; sender_index++) {
        std::uint64_t id = NetworkSimulator::GenerateNewID();
        senders.emplace_back(senderFactory(id, receiver_id, sender_index, senderArgs...));
        senders_ids.push_back(id);
    }
    
    std::shared_ptr<NetworkSwitch> n_switch(new Switch(switch_buffer_size, switch_process_time));
    NetworkSimulator simulator(senders, receiver, n_switch, simulation_duration);

    simulator.StartSimulation();
    return receiver->ToString();
}

template <typename... SenderArgs>
std::function<std::shared_ptr<Sender>(std::uint64_t, std::uint64_t, int, SenderArgs...)> senderFactory = [](std::uint64_t id, std::uint64_t receiver_id, int index, std::uint64_t sending_time_offset, std::uint64_t sending_time_interval) {
    return std::make_shared<Sender>(id, receiver_id, sending_time_offset * index, sending_time_interval);
};

template <typename... SenderMultiplierArgs>
std::function<std::shared_ptr<SenderWithMultiplier>(std::uint64_t, std::uint64_t, int, SenderMultiplierArgs...)>  senderMultiplierFactory = [](std::uint64_t id, std::uint64_t receiver_id, int index, std::uint32_t multiplier, std::uint64_t sending_time_interval) {
    return std::make_shared<SenderWithMultiplier>(id, receiver_id, multiplier, sending_time_interval);
};

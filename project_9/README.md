## Проект 9: Алгоритм контроля перегрузки Backpressure на C++
## Студент: Левченко Богдан

### Build

```bash
mkdir build && cd build
```
```bash
cmake ..
```
```bash
cmake --build .
```
Run program:
```bash
./express_pass
```

### Simulator usage example

```cpp
int main() {
    // Setup amount of servers
    std::uint32_t amount_of_senders = 10;

    // Initialize senders, receiver and switch
    // Reminder: use NetworkSimulator::GenerateNewID() for getting hosts ids
    std::vector<std::shared_ptr<ServerBase>> senders = ...;
    std::shared_ptr<ServerBase> receiver = ...;
    std::shared_ptr<NetworkSwitch> n_switch = ...;

    // Init and start simulation
    NetworkSimulator simulator(senders, receiver, n_switch, 100000);
    simulator.StartSimulation();
}
```


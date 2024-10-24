## Проект 1: Моделирование перегрузки сети в дата-центре на C++
## Студент: Иван Шаныгин

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
./datacenter_overloading
```

### Usage

```cpp
int main() {
    std::uint32_t number_of_servers = 3;

    // Create simulator
    NetworkSimulator simulator(number_of_servers);

    // Vector with distances from sending servers to receiving server
    std::vector<std::uint32_t> distances_to_receiver = {10, 4, 20};

    // Start simulation
    simulator.StartSimulation(distances_to_receiver);

    // Print information about simulator's configuration
    simulator.PrintConfiguration();
}
```


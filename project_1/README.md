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
    std::uint32_t simulation_time_sec = 1;
    std::vector<std::uint32_t> distances_to_receiver_us = {10, 4, 20};

    // Create simulator with vector of distances from senders to receiver
    NetworkSimulator simulator(distances_to_receiver_us);

    // Start simulation with duration simulation_time_sec
    simulator.StartSimulation(simulation_time_sec);

    // Print information about simulator's configuration
    std::cout << simulator;
}
```

### Data

Data stored in `data` directory

### Data visualization

Jupyter notebook with Python code for data visualization stored in `data_visualization` directory.
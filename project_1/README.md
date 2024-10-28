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
    // Create simulator
    // {10, 4, 20} -- distances (in μs) from senders to receiver
    NetworkSimulator simulator({10, 4, 20});

    // Start simulation
    simulator.StartSimulation();

    // Print information about simulator's configuration
    std::cout << simulator;
}
```


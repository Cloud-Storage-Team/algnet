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
Run **program**:
```bash
./network_simulator
```
Run **tests**:
```bash
./network_simulator_test
```

### Usage

```cpp
int main() {
    // Vector with distances in nanoseconds from source servers to destination server
    std::vector<std::uint32_t> distances_to_receiver_ns = {500, 1000, 2000};
    
    // Create instance of Network Simulator
    NetworkSimulator ns(distances_to_receiver_ns);
    // Run simulation
    ns.Run();
}
```
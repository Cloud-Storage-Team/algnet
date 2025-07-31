# Not Overcomplicated Network Simulator

## Overview
Not Overcomplicated Network Simulator (NoNS) is a free open source project aiming to build a discrete-event network simulator targeted at easy testing of congestion control algorithms.

## Build project
NoNS uses CMake build manager, so project builds in this way:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Run project

```
./build/nons
    --config path
    [--output-dir output-dir-name]
    [--no-logs]
    [--no-plots]
    [--export-metrics]
```

Options:

```
-c, --config arg          Path to the simulation configuration file
    --output-dir arg      Output directory for metrics and plots
                        (default: metrics)
    --no-logs             Output without logs
    --no-plots            Disables plots generation
    --metrics-filter arg  Fiter for collecting metrics pathes
                        (default: .*)
-h, --help                Print usage
```

Examples of simulation configs placed in `configuration_examples/simulation_examples`

### `export-metrics` flag format

This flags represents regular expression that match generated **data file names** under metrics output directory. Plots generates accordingly to collected data.

E.g. if `--metrics-filter = "cwnd/.*"`, NoNS measures only CWND values, if `--metrics-filter = ".*_link1.*"`, only metircs about link1.

## How to add new congestion control algorithm

If you want to implement TCP-like algorithm, follow these steps:

1. Create class (`YourCC` further) that implements [`ITcpCC`](source/flow/tcp/i_tcp_cc.hpp). It should be a class that contains all logic of your congestion control algorithm. See example: [`TcpAIMDCC`](source/flow/tcp/aimd/tcp_aimd_cc.cpp).
2. Add alias for `TcpFlow<YourCC>` (`YourTcp` further). Its is recomended to put it in the same directory with `YourCC`.
3. Add implememtation for `Parser<YourTcp>::parse_tcp_cc`. It is a function that parses `YourCC` from yaml file. See [implementation](source/parser/identifiable_parser/flow/parse_tcp_aimd_cc.cpp) for `TchAIMD`
4. Add alias (`YourSmulator` further) for simulator class based on `YourTcp` and put it in the end of [simulator.hpp](source/simulator.hpp) file.
5. Add parsing of `YourSmulator` in [`create_simulator` function](source/simulator.cpp). The string you match `algorithm` will be used in simulation config. See [simulation config](configuration_examples/simulation_examples/tcp_simulation.yml) for `TcpAIMD` algorithm. 

If your algorithm is not TCP-like (e.g. credit-based), do fillowing:
1. Create class (`YourFlow` further) that implements [`IFllow`](source/flow/i_flow.hpp). `YourFlow` should repersent all logic of sending packets. See example: [`TcpFlow`](source/flow/tcp/tcp_flow.hpp).
3. Add implememtation for `Parser<YourFlow>::parse_object`. This function should parse `YourFlow` from yaml file. See [implementation](source/parser/identifiable_parser/flow/parse_tcp_flow.hpp) for `TcpFlow`
4. Add alias (`YourSmulator` further) for simulator class based on `YourFlow` and put it in the end of [simulator.hpp](source/simulator.hpp) file.
5. Add parsing of `YourSmulator` in [`create_simulator` function](source/simulator.cpp). The string you match `algorithm` will be used in simulation config. See [simulation config](configuration_examples/simulation_examples/tcp_simulation.yml) for `TcpAIMD` algorithm. 

## Resuls of simulations

Metrics and results of load testing of all simulation runs deploys to [gihub pages](https://cloud-storage-team.github.io/algnet)
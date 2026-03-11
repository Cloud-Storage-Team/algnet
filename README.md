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
```

Options:

```
-c, --config arg          Path to the simulation configuration file
    --output-dir arg      Output directory for metrics and plots
                        (default: metrics)
    --no-logs             Output without logs
-h, --help                Print usage
```

Examples of simulation configs placed in `configuration_examples/simulation_examples`

### `metrics-filter` flag format

These flags represent regular expression that match generated **data file names** under metrics output directory. Plots generates accordingly to collected data.

E.g. if `--metrics-filter = "cwnd/.*"`, NoNS measures only CWND values, if `--metrics-filter = ".*_link1.*"`, only metircs about link1.

## How to add a new congestion control algorithm

If you want to implement TCP-like algorithm, follow these steps:

1. Create class (`YourCC` further) that implements [`ITcpCC`](source/connection/flow/tcp/i_tcp_cc.hpp). It should be a class that contains all logic of your congestion control algorithm. See example: [`TcpTahoeCC`](source/connection/flow/tcp/tahoe/tcp_tahoe_cc.cpp).
2. Add implementation of parsing `YourCC` and put it to [directory](source/parser/simulation/connection/flow/tcp/cc/) with tcp congestion control parsers and call it from [`common TCP congestion control parser`](source/parser/simulation/connection/flow/tcp/cc/tcp_cc_parser.cpp)

## Results of simulations

Metrics and results of load testing of all simulation runs are deployed to [GitHub Pages](https://cloud-storage-team.github.io/algnet)

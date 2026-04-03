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
-c, --config arg          Path to the scenario configuration file
    --output-dir arg      Output directory for metrics and plots
                        (default: metrics)
    --no-logs             Output without logs
-h, --help                Print usage
```

Examples of configs are placed under [`configs`](configs/) directory. See scenario format description [here](configs/_docs/scneario-config.md).

## How to add a new congestion control algorithm

If you want to implement TCP-like algorithm, follow these steps:

1. Create class (`YourCC` further) that implements [`ITcpCC`](source/network/connection/mplb/cc/i_tcp_cc.hpp). It should be a class that contains all logic of your congestion control algorithm. See example: [`TcpTahoeCC`](source/network/connection/mplb/cc/tahoe/tcp_tahoe_cc.cpp).
2. Add implementation of parsing `YourCC` and put it to [directory](source/network/connection/mplb/cc/) with tcp congestion control parsers and call it from [`common TCP congestion control parser`](source/parser/network/connection/mplb/cc/tcp_cc_parser.hpp)

## Results of simulations

Metrics and results of load testing of all simulation runs are deployed to [GitHub Pages](https://cloud-storage-team.github.io/algnet)

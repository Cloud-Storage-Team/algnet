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
-c, --config arg      Path to the simulation configuration file
    --output-dir arg  Output directory for metrics and plots (default: metrics)
    --no-logs         Disables logging
    --no-plots        Disables plots generation
    --export-metrics  Export metric values into output-dir
-h, --help            Print usage
```

Examples of simulation configs placed in `configuration_examples/simulation_examples`

## Resuls of simulations

Metrics of all simulation runs deploys to [gihub pages](https://cloud-storage-team.github.io/algnet/main). If you want to see results on all branches, visit [root site](https://cloud-storage-team.github.io/algnet).

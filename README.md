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

## Resuls of simulations

Metrics and results of load testing of all simulation runs deploys to [gihub pages](https://cloud-storage-team.github.io/algnet)
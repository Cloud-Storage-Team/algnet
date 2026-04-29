# Not Overcomplicated Network Simulator

## Overview
Not Overcomplicated Network Simulator (NoNS) is a free open source project aiming to build a discrete-event network simulator targeted at easy testing of congestion control algorithms.

## Dependencies

NoNS need gnuplot & graphiz to generate plots, so install it:
```
sudo apt install --fix-missing gnuplot
sudo apt install --fix-missing graphviz
```

Other dependecies are designed as git submodules, so to install them, run
```
git submodule init
git submodule update --init --recursive
```

## Build project
NoNS uses CMake build manager, so project builds in this way:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```


## Run project

Build project and run it with `-h` option to see help.

Examples of configs that need to run NoNS are placed under [`configs`](configs/) directory. See config schemas [here](_schemas/configs/).

## How to add a new congestion control algorithm

If you want to implement TCP-like algorithm, follow these steps:

1. Create class (`YourCC` further) that implements [`ITcpCC`](source/network/connection/mplb/cc/i_tcp_cc.hpp). It should be a class that contains all logic of your congestion control algorithm. See example: [`TcpTahoeCC`](source/network/connection/mplb/cc/tahoe/tcp_tahoe_cc.cpp).
2. Add implementation of parsing `YourCC` and put it to [directory](source/network/connection/mplb/cc/) with tcp congestion control parsers and call it from [`common TCP congestion control parser`](source/parser/network/connection/mplb/cc/tcp_cc_parser.hpp)

## Results of simulations

Metrics and results of load testing of all simulation runs are deployed to [GitHub Pages](https://cloud-storage-team.github.io/algnet)

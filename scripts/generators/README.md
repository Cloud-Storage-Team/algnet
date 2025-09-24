# Config generators overview

There are three types of generators:
- [Topology generators](topology/README.md) - generates topology
- [Simulation generators](simulation/README.md) - generates both topology and simulation
- [Topology independent (TI) simulation generators](ti_simulation/README.md) - generates simulation config for any given topology.   

Each generator should a directory with at least two files: python script and yaml config. 

So yaml config in your generator's directory should be correct config for generator (also a good practice is to make it default).
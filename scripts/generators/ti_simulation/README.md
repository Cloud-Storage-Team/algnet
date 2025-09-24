# Topology independent (TI) simulation generators

Each generator in this directory should be able to generate simulation config for any topology. Moreover, each of them should have **exaclty three argumetns**:
- `-t (--topology)` - path to topology for which needs to generate simulation
- `-c` `(--config)` - path to config file that contains remaining generator parameters (presets etc). **This field should have default value, correct for every script launch place**
- `-o` `(--output_path)` - path to output config
# Simulation config generators

Each generator in this directory should generate simulation config. Moreover, each of them should have **exaclty two argumetns**:
- `-c` `(--config)` - path to config file that contains generator parameters (presets etc). **This field should have default value, correct for every script launch place**
- `-o` `(--output_path)` - path to output config
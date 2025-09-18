# Config generators overview

There are two important directories: `simulation` and `topology`. Each of them consist of config generators (simulation configs & topology configs accordingly).

Each generator should a directory with at least two files: python script and yaml config. Python script **must have exactly two paraments:**
- `-c` `(--config)` - path to config file that contains all generators parameters (usually consist of some presets and other settings). **This field should have default value, correct for every script launch place**
- `-o` `(--output_path)` - path to output config

So yaml config in your generator's directory should be correct config for generator (also a good practice is to do it default).  

## How to organize not universal simulation generator

If your simulation config generator should generate topology too, you may place simulation and topology parts into one output file. In this sittuation set `topology_config_path` to name of file you save config (or any other connrect path to it).
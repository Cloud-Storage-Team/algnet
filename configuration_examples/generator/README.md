# How to generate config

### Incast topology

To generate simulation and topology configs for 50-to-1 incast topology just run this command from root directory:

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders 50 \
  --receivers 1 \
  --packets 10000 \
  --simulation-time 1000000
  --topology incast-50-to-1-topology.yml \
  --topology-dir configuration_examples/generator/ \
  --simulation incast-50-to-1-simulation.yml \
  --simulation-dir configuration_examples/generator/
```

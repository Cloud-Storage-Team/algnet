# How to generate config

### Incast topology N-to-1

To generate simulation and topology configs for **N-to-1 incast** topology just run this command from root directory:

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders N \ # N should be replaced with a number
  --receivers 1 \
  --packets 10000 \
  --simulation-time 1000000
  --topology incast-50-to-1-topology.yml \
  --topology-dir configuration_examples/generator/ \
  --simulation incast-50-to-1-simulation.yml \
  --simulation-dir configuration_examples/generator/
```

### Incast topology M-to-N

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders M \ # M should be replaced with a number
  --receivers N \ # N should be replaced with a number
  --flows 1-to-all \ # Total M*N flows
  --packets 10000 \
  --simulation-time 1000000
  --topology incast-50-to-1-topology.yml \
  --topology-dir configuration_examples/generator/ \
  --simulation incast-50-to-1-simulation.yml \
  --simulation-dir configuration_examples/generator/
```

# Usage examples

All the following commands must be run from root directory!

### Incast topology N-to-1

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders N \ # N should be replaced with a number
  --receivers 1 \
  --packets 10000 \
  --simulation-time 1000000 \
  --topology incast-N-to-1-topology.yml \
  --topology-dir configuration_examples/generator/ \
  --simulation incast-N-to-1-simulation.yml \
  --simulation-dir configuration_examples/generator/
```

### Topology N-to-N, N flows

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders N \ # N should be replaced with a number
  --receivers N \ # N should be replaced with a number
  --packets 10000 \
  --simulation-time 1000000 \
  --topology incast-N-to-N-topology.yml \
  --topology-dir configuration_examples/generator/ \
  --simulation incast-N-to-N-simulation.yml \
  --simulation-dir configuration_examples/generator/
```

### Topology M-to-N, M*N flows

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders M \ # M should be replaced with a number
  --receivers N \ # N should be replaced with a number
  --flows all-to-all \ # Total M*N flows
  --packets 10000 \
  --simulation-time 1000000 \
  --topology incast-M-to-N-topology.yml \
  --topology-dir configuration_examples/generator/ \
  --simulation incast-M-to-N-simulation.yml \
  --simulation-dir configuration_examples/generator/
```

### Topology with several switches between senders and receivers

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders M \ # M should be replaced with a number
  --receivers N \ # N should be replaced with a number
  --switches 2 \
  --flows 1-to-all \
  --packets 10000 \
  --simulation-time 1000000 \
  --topology incast-M-to-N-topology.yml \
  --topology-dir configuration_examples/generator/ \
  --simulation incast-M-to-N-simulation.yml \
  --simulation-dir configuration_examples/generator/

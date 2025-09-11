# Usage examples

`bottleneck.py`

```bash
Generate topology and simulation YAML files.

options:
  -h, --help            show this help message and exit
  --senders SENDERS     Number of sender devices
  --receivers RECEIVERS
                        Number of receiver devices
  --switches SWITCHES   Number of switches between senders and receivers. 
                        Default value: 1
  --topology-path TOPOLOGY_PATH
                        Path to the output topology config file
                        Default value: bottleneck_topology.yml
  --simulation-path SIMULATION_PATH
                        Path to the output simulation config file. 
                        Default value: bottleneck_simulation.yml
  --simulation-time SIMULATION_TIME
                        Time of the simulation, ns. 
                        Default value: None
  --packets PACKETS     Number of packets sending by each sender. 
                        Default value: 100
  --packet-interval PACKET_INTERVAL
                        Time between two consequent packets, ns
  --flows FLOWS         Flows: 1-to-1 or all-to-all. 
                        Default value: 1-to-1
```

#### All the following commands must be run from root directory!

### Incast topology N-to-1

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders N \ # N should be replaced with a number
  --receivers 1 \
  --packets 10000 \
  --simulation-time 1000000 \
  --topology-path configuration_examples/generator/incast-N-to-1-topology.yml \
  --simulation-path configuration_examples/generator/incast-N-to-1-simulation.yml
```

### Topology N-to-N, N flows

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders N \ # N should be replaced with a number
  --receivers N \ # N should be replaced with a number
  --packets 10000 \
  --simulation-time 1000000 \
  --topology-path configuration_examples/generator/incast-N-to-N-topology.yml \
  --simulation-path configuration_examples/generator/incast-N-to-N-simulation.yml
```

### Topology M-to-N, M*N flows

```bash
python3 configuration_examples/generator/bottleneck.py \
  --senders M \ # M should be replaced with a number
  --receivers N \ # N should be replaced with a number
  --flows all-to-all \ # Total M*N flows
  --packets 10000 \
  --simulation-time 1000000 \
  --topology-path configuration_examples/generator/incast-M-to-N-topology.yml \
  --simulation-path configuration_examples/generator/incast-M-to-N-simulation.yml
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
  --topology-path configuration_examples/generator/incast-M-to-N-topology.yml \
  --simulation-path configuration_examples/generator/incast-M-to-N-simulation.yml
```

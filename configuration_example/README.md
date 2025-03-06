# Datacenter Topology File Format

This document describes the structure of the YAML file used to define datacenter network topologies, including hosts,
switches, and links between them.

## Structure Overview

The topology file is divided into multiple sections, each representing a different network topology. Each topology
consists of:

- `hosts`: Defines sender and receiver devices.

- `switches`: Defines network switches.

- `links`: Specifies connections between devices with network parameters.

## Sections

### Hosts

Each host entry represents either a sender or a receiver:

```yaml
hosts:
  host_id:
    type: [ sender | receiver ]
    name: <device_name>
```

- `host_id`: Unique identifier for the host.

- `type`: Defines whether the device is a sender or receiver.

- `name`: Descriptive name for the device.

### Switches

Switches serve as interconnects between hosts:

```yaml
switches:
  switch_id:
    type: switch
    name: <switch_name>
```

- `switch_id`: Unique identifier for the switch.

- `type`: Always set to switch.

- `name`: Descriptive name for the switch.

### Links

Each link defines a connection between two devices:

```yaml
links:
  link_id:
    from: <source_device>
    to: <destination_device>
    latency: <value><unit>
    throughput: <value><unit>
```

- `link_id`: Unique identifier for the link.

- `from`: Source device (host or switch).

- `to`: Destination device (host or switch).

- `latency`: Transmission delay (e.g., 5ms).

- `throughput`: Link bandwidth (e.g., 1Gbps).

### Examples images

[Images](images) of topologies are generated using the [generate.py](images/generator.py) script.
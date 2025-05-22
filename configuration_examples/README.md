# Datacenter Topology File Format

This document describes the structure of the YAML file used to define datacenter network topologies, including hosts,
switches, and links between them.

## Structure Overview

The topology file is divided into multiple sections, each representing a different network topology. Each topology
consists of:

- `devices`: Defines devices: senders, receivers and switches.

- `links`: Specifies connections between devices with network parameters.

## Sections

### Devices

Each host entry represents a device:

```yaml
devices:
  device_name:
    type: [ sender | receiver | switch ]
```

- `device_name`: Unique identifier for the device.

- `type`: Defines device type.

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

- `from`: Source device.

- `to`: Destination device.

- `latency`: Transmission delay (e.g., 5ns).

- `throughput`: Link bandwidth (e.g., 1Gbps).

### Examples images

[Images](images) of topologies are generated using the [generate.py](images/generator.py) script.

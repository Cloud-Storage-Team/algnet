# Datacenter Topology Config Format

This document describes the structure of the YAML file used to define datacenter network topologies, including hosts,
switches, and links between them.

## Structure Overview

The topology file is divided into multiple sections, each representing a different network topology. Each topology
consists of:

- `packet-spraying`: defines packet spraying type and its setting is need

- `hosts`: Defines hosts

- `switches` : Defines switches

- `links`: Specifies connections between devices (hosts and switches) with network parameters.

## Sections

### Packet spraying

Structure:
```yaml
packet-spraying:
  type: random|ecmp|salt|flowlet 
```

If type is flowlet, it needs to set field `threshold` in [time format](../README.md)

### Hosts

Each hosts entry represents a host:

```yaml
hosts:
  host_name: 
```

`host_name` should be unique 

### Switches

Each swtiches entry represents a switch:

```yaml
switches:
  switch_name:
    ecn:
      min:
      max:
      probability:
```
- `switch_name` is unique name of switch
- `ecn` is optional and describes settings Explicit Congestion Notification settings. You can see more about it [here](https://man7.org/linux/man-pages/man8/tc-red.8.html)  

### Links

Each link defines a connection between two devices:

```yaml
links:
  link_id:
    from: <source_device>
    to: <destination_device>
    latency: <value>
    throughput: <value>
    ingress_buffer_size: <value>
    egress_buffer_size: <value>
```

- `link_id`: Unique identifier for the link.

- `from`: Source device name.

- `to`: Destination device name.

- `latency`: Transmission delay in [time format](../README.md)

- `throughput`: Link bandwidth in [speed format](../README.md)

- `ingress_buffer_size`: Link ingress buffer size in [size format](../README.md)

- `egress_buffer_size`: Link egress buffer size in [size format](../README.md)

### Examples images

You may generate images of topologies using the [generator](../../scripts/generate_image.py) script.

# Datacenter Topology Simulation Config Format

This document describes the structure of the YAML file used to define datacenter simulation config format, including connections, flows other settings.

## Structure Overview

The topology file is divided into multiple sections:

- `topology_config_path`: path (absolute or relative) to the topology config.

- `simulation_time`: maximal time of simulation in [time format](../README.md); optional field

- `connections` : Defines connections

## Connections

`connections` sections looks like:

```yaml
connections:
  sender_id: <source_device>
  receiver_id: <destination_device>
  data_to_send: <size>
  mpld: round_robin
  flows:
    # flows section; see it below
```

- `sender_id` and receiver_id should be the names of hosts  from topology config.
- `data_to_send` is an number in [size format](../README.md)
- `mplb` is multipath load ballansing type. The only availabe value is `round_robin`

# Flows section

Each flow describes flow from `source_id` to `receiver_id`:

```yaml
flows:
  flow_id:
    type: tcp
      packet_size: <size>
      cc:
        # congestion control settings; see it below
```

- `flow_id` is an unique name of flow (names of flows from different connections may be equal)
- `type` is a type of flow. The only avaliable value is tcp
- `packet_size` is an number in [size format](../README.md)
- `cc` describes congestion control module of flow. See more details below

# `cc` section in tcp flow

`cc` module looks like:

```yaml
cc:
  type: basic|tahoe|swift
  # rest values specific for given congestion control type
```

`type` describes type of congestion control:

- `basic`: no congestion control; no specific values
- `tahoe`: [tcp tahoe](https://www.geeksforgeeks.org/computer-networks/tcp-tahoe-and-tcp-reno/) congestion control; no specific values
- `swift`: [swift](https://2022-cs244.github.io/papers/L5-swift.pdf) congestion control (especialle see pseudocode on page 516); one specific value:
  + `target_delay`: value from paper above in [time format](../README.md)

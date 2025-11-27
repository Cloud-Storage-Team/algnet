# Datacenter Topology Simulation Config Format

This document describes the structure of the YAML file used to define datacenter simulation config format, including connections, flows, scenario and other settings.

## Structure Overview

The topology file is divided into multiple sections:

- `topology_config_path`: path (absolute or relative) to the topology config.

- `simulation_time`: maximal time of simulation in [time format](../README.md); optional field

- `connections` : Defines connections

- `scenario` : Defines a custom usage scenario. For example, how much data to send at a given time

## Connections

`connections` sections looks like:

```yaml
connections:
  sender_id: <source_device>
  receiver_id: <destination_device>
  mplb: round_robin
  flows:
    # flows section; see it below
```

- `sender_id` and receiver_id should be the names of hosts  from topology config.
- `mplb` is multipath load balansing type. The available value is `round_robin` or `srct`

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
- `type` is a type of flow. The only available value is tcp
- `packet_size` is an number in [size format](../README.md)
- `cc` describes congestion control module of flow. See more details below

# `cc` section in tcp flow

`cc` module looks like:

```yaml
cc:
  type: basic|tahoe|swift
  # rest fields specific for given congestion control type
```

`type` describes type of congestion control:

- `basic`: no congestion control; no specific fields
- `tahoe`: [tcp tahoe](https://www.geeksforgeeks.org/computer-networks/tcp-tahoe-and-tcp-reno/) congestion control; spicific optional fields (all are real numbers):
  + `start_cwnd`: Initial cwnd value; default: `1.0`
  + `sstresh`: Initial sstresh value; default: `8.0`
- `swift`: [swift](https://2022-cs244.github.io/papers/L5-swift.pdf) congestion control (especially see pseudocode on page 516); specific fields:
  
  Required:
    + `base_target`: value from paper above in [time format](../README.md)

  Optional (all are real numbers):
    + `start_cwnd`: initial congestion window size; default: `1`.
    + `ai`: Additive‑increase constant; default: `0.5`.
    + `md`: Multiplicative decrease constant; default: `0.5`.
    + `max_mdf`: Upper bound on MD factor per RTT; default: `0.3`.
    + `fs_range`: Max queue head‑room (flow‑scaling term); default: `0.3`.
    + `fs_min_cwnd`: Cwnd where flow‑scaling hits fs_range; default: `0.1`.
    + `fs_max_cwnd`: Cwnd where flow‑scaling goes to zero; default: `100`.

# Scenario section

The `scenario` section describes a sequence of actions that will be executed during the simulation.

Example:

```yaml
scenario:
  - action: send_data
    when: 0ns
    size: 1024B
    connections: conn1
    repeat_count: 5
    repeat_interval: 100000ns
```


`action` — the type of action. This field is **mandatory**.
- Other fields depend on the chosen `action` type.  
- At the moment, only `send_data` and `stop_time` is supported. Additional actions will be introduced in the future.

`send_data` action:  
Specifies how much data should be sent to the connection level at a given time.  
**mandatory fields:**
- `when`: The time at which the data will start being sent in [time format](../README.md) 
- `size`: The amount of data to send in [size format](../README.md)
- `connections`: Specifies which connections to use for sending data. Supports regular expression

**optional fields:**
- `repeat_count`: Sets the number of times to perform an action (1 by default)
- `repeat_interval` : Sets the time interval after which actions will be performed in [time format](../README.md)

`stop_time` action:
Specifies the time to stop the simulation.
**mandatory field:**
- `time`: the time of stop [time format](../README.md)
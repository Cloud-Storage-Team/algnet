# General configs information

## Units data format

In all configs you may see fields that contains time, size and speed. All of them have same format: `<value><unit>` where `value` is an integer and `unit` is small string suffix. For each unit where are fixed list of avaliable units that you may see below.

### Avaliable time units:

- `ns`: Nanosecond
- `us`: Microsecond
- `ms`: Milisecond
- `s`: Second

### Avaliable size units:

- `b`: bit
- `B`: Byte
- `Mb`: Megabit
- `MB`: Megabyte
- `Gb`: Gigabit
- `GB`: Gigabyte

### Avaliable speed units:

- `Mbps`: Megabits per second
- `Gbps`: Gigabits per second

Examples:
- Time: `1ns`, `2s`,
- Size: `1Mb`, `5GB`
- Speed: `2Gpbs`, `3Mbps`

## Presets

For some config parts you may create presets - common set of fields for the objects. Foe example, in topology configs it may looks like
```yaml
presets:
  links:
    best-link:
      latency: 1000ns
      throughput: 10Gbps
      ingress_buffer_size: 4096B
      egress_buffer_size: 4096B
  switches:
    really-best-switch:
      ecn:
        min: 0.2
        max: 0.4
        probability: 0.8
    bad-switch:
      ecn:
        min: 1.0
        max: 1.0
        probability: 0.0
```

To use your preset, fill in `preset-name` feild in cocrete object description:

```yaml
switches:
  switch-1:
    preset-name: really-best-switch
  switch-2:
    preset-name: bad-switch
links:
  link1-2:
    preset-name:  best-link
    from: switch-1
    to: switch-2
  link2-1:
    preset-name: best-link
    from: switch-2
    to: switch-1
    throughput: 10Gpps # overrides value from preset
```

## Default preset

To shorten the configs length, you may fill default preset that used for all objects in description of which `preset-name` does not set (or set to `default`):

```yaml
presets:
  links:
    default:
      latency: 1000ns
      throughput: 10Gbps
      ingress_buffer_size: 4096B
      egress_buffer_size: 4096B
links:
   link-1:
      from: device-1
      to: device-2
      # other fields takes from default preset
    link-1:
      preset-name: default # works similary with link-1
      from: device-2
      to: device-1

```

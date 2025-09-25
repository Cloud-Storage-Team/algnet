# Fractal topology generator

Generates topology that consis of some consistent layers:
- First layer contains the only one device `sender`
- Each of next `depth` layers is 'switch' layer. It turns out as follows: for every device from previous layer creates `num_switches_per_device` switches; them connects to this device with bidirectional link
- Last layer contains the only one device `receiver` that connected by bidirectional links with all swtiches from last 'switch' layer   

So for `depth = 2` and `num_switches_per_device = 3` you will get following topology:

![](fractal_2_3.svg)
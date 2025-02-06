import matplotlib.pyplot as plt
import sys

time = []
data = []

with open(sys.argv[1], 'r') as file:
    for line in file:
        parts = line.strip().split()
        if len(parts) == 2:
            try:
                t, c = map(float, parts)
                time.append(t)
                data.append(c)
                if t >= float(sys.argv[2]):
                    break
            except ValueError:
                continue

plt.figure(figsize=(15, 9))
plt.plot(time, data, marker='o', linestyle='-', color='blue')
plt.grid(True, linestyle='--', alpha=0.7)
plt.savefig(sys.argv[1][:-3] + "png", format='png')
plt.close()

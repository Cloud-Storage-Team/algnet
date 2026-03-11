hosts = [f"pod{pod}_host{host}" for pod in range(1, 5) for host in range(1, 5)]

print("connections:")
for h1 in hosts:
    for h2 in hosts:
        if h1 == h2:
            continue
        print(f"  conn_{h1}--{h2}:")
        print(f"    sender_id: {h1}")
        print(f"    receiver_id: {h2}")
        print(f"    preset_name: default_connection")


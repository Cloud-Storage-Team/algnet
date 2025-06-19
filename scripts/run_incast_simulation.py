import sys
import os
import subprocess
import argparse
import time


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Generate incast topologies and simulate them."
    )
    parser.add_argument(
        "--senders", type=int, help="Number of senders in topology", default=30
    )
    parser.add_argument(
        "--receivers", type=int, help="Number of receivers in topology", default=1
    )
    parser.add_argument("--time", type=int, help="Simulation time, ns", default=10000)
    parser.add_argument(
        "--packets", type=int, help="Number of sending packets", default=100
    )

    args = parser.parse_args()

    # Validate inputs
    if args.senders < 1:
        print("Error: Number of senders must be at least 1", file=sys.stderr)
        sys.exit(1)
    if args.receivers < 1:
        print("Error: Number of receivers must be at least 1", file=sys.stderr)
        sys.exit(1)
    if args.packets < 1:
        print("Error: Number of packets must be at least 1", file=sys.stderr)
        sys.exit(1)
    if args.time < 1:
        print("Error: Simulation time must be at least 1ns", file=sys.stderr)
        sys.exit(1)

    return args


def main(args):
    parsed_args = parse_arguments()
    topology_config_name = (
        f"incast-{parsed_args.senders}-to-{parsed_args.receivers}-topology.yml"
    )
    simulation_config_name = (
        f"incast_{parsed_args.senders}_to_{parsed_args.receivers}_simulation.yml"
    )

    bottleneck_args = [
        "python3",
        "bottleneck.py",
        "--senders",
        str(parsed_args.senders),
        "--receivers",
        str(parsed_args.receivers),
        "--topology",
        topology_config_name,
        "--simulation",
        simulation_config_name,
        "--packets",
        str(parsed_args.packets),
        "--simulation-time",
        str(parsed_args.time),
    ]

    os.chdir("configuration_examples/generator")
    bottleneck_result = subprocess.run(bottleneck_args, capture_output=True)
    if bottleneck_result.returncode != 0:
        print(f"Error in bottleneck.py.")
        print(f"Output: {bottleneck_result.stderr.decode()}")
        os.chdir("../..")
        exit(1)

    simulator_args = [
        "time",
        "./build/simulator",
        "--config",
        f"configuration_examples/simulation_examples/{simulation_config_name}",
        "--no-logs",
        "--no-plots",
    ]

    print(
        f"Running simulation with {parsed_args.senders} senders, {parsed_args.receivers} receivers, {parsed_args.packets} packets, {parsed_args.time}ns"
    )

    os.chdir("../..")
    start_time = time.perf_counter()
    simulator_result = subprocess.run(simulator_args, capture_output=True)
    elapsed_time = time.perf_counter() - start_time
    if simulator_result.returncode != 0:
        print(f"Error running simulator.")
        print(f"Simulator output: {simulator_result.stderr.decode()}")
        exit(1)
    print(f"Elapsed time: {elapsed_time:.3f} sec")


if __name__ == "__main__":
    main(sys.argv)

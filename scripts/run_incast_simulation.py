import sys
import os
import subprocess
import argparse


def main(args):
    parser = argparse.ArgumentParser()
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
    parsed_args = parser.parse_args()

    topology_config_name = (
        f"incast-{parsed_args.senders}-to-{parsed_args.receivers}-topology.yml"
    )
    simulation_config_name = (
        f"incast_{parsed_args.senders}_to_{parsed_args.receivers}_simulation.yml"
    )

    bottleneck_args = [
        "python3",
        "configuration_examples/generator/bottleneck.py",
        "--senders",
        parsed_args.senders,
        "--receivers",
        parsed_args.receivers,
        "--topology",
        topology_config_name,
        "--simulation",
        simulation_config_name,
        "--packets",
        parsed_args.packets,
        "--simulation-time",
        parsed_args.time,
    ]

    bottleneck_result = subprocess.run(bottleneck_args, capture_output=True)
    if bottleneck_result.returncode != 0:
        print(f"Error in bottleneck.py.")
        print(f"Output: {bottleneck_result.stderr.decode()}")
        exit(1)

    simulator_args = [
        "time",
        "./build/simulator",
        f"configuration_examples/simulation_examples/{simulation_config_name}",
        "--no-logs",
        "--no-plots",
    ]

    print(
        f"Running simulation with {parsed_args.senders} senders, {parsed_args.receivers} receivers, {parsed_args.packets} packets, {parsed_args.time}ns"
    )

    simulator_result = subprocess.run(simulator_args, capture_output=True)
    if simulator_result.returncode != 0:
        print(f"Error running simulator.")
        print(f"Simulator output: {simulator_result.stderr.decode()}")
        exit(1)


if __name__ == "__main__":
    main(sys.argv)

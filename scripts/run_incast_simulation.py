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
        "-e", "--executable", help="Path to NoNS executable file", required=True
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
    parser.add_argument(
        "-o", "--output-dir", help="Path to directory for generated configs", default=""
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
        f"incast_{parsed_args.senders}_to_{parsed_args.receivers}_topology.yml"
    )
    simulation_config_name = (
        f"incast_{parsed_args.senders}_to_{parsed_args.receivers}_simulation.yml"
    )
    bottleneck_script_path = os.path.join("scripts", "topology_generator", "bottleneck.py")

    bottleneck_args = [
        "python3",
        bottleneck_script_path,
        "--senders",
        str(parsed_args.senders),
        "--receivers",
        str(parsed_args.receivers),
        "--topology-path",
        os.path.join(parsed_args.output_dir, topology_config_name),
        "--simulation-path",
        os.path.join(parsed_args.output_dir, simulation_config_name),
        "--packets",
        str(parsed_args.packets),
        "--simulation-time",
        str(parsed_args.time),
    ]

    subprocess.run(bottleneck_args, capture_output=True, check=True)

    simulator_args = [
        "time",
        str(parsed_args.executable),
        "--config",
        str(os.path.join(parsed_args.output_dir, simulation_config_name)),
        "--no-logs",
        "--no-plots",
    ]

    print(
        f"Running simulation with {parsed_args.senders} senders, {parsed_args.receivers} receivers, {parsed_args.packets} packets, {parsed_args.time}ns"
    )

    start_time = time.perf_counter()
    subprocess.run(simulator_args, capture_output=True, check=True)
    elapsed_time = time.perf_counter() - start_time
    print(f"Elapsed time: {elapsed_time:.3f} sec")


if __name__ == "__main__":
    main(sys.argv)

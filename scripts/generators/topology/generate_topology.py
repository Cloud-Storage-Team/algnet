from common import run_subprocess

def generate_topology(topology_generator_path : str, output_file : str, config_path : str | None = None):
    """
    Generates topology config using given generator (topology_generator_path)
    on given config (or use default if config_path is None).
    Puts generated topology config to output_file.
    Returns result of generator run
    """
    args = [
        "python3",
        topology_generator_path,
        "-o",
        output_file
    ]
    if config_path is not None:
        args += ["-c", config_path]
    return run_subprocess(args)
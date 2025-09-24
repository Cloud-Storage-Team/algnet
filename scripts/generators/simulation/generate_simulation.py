from common import run_subprocess

def generate_simulation(simulation_generator_path : str, output_file : str, config_path : str | None = None):
    """
    Generates simulation config using given generator (simulation_generator_path)
    on given config (or use default if config_path is None).
    Puts generated simulation config to output_file.
    Returns result of generator run
    """
    args = [
        "python3",
        simulation_generator_path,
        "-o",
        output_file
    ]
    if config_path is not None:
        args += ["-c", config_path]
    return run_subprocess(args)
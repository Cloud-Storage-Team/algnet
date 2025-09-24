from common import run_subprocess

def generate_simulation_by_topology(ti_sumlation_generator_path : str, topology_path : str, output_file : str, config_path : str | None = None):
    """
    Generates simulation based on topology (located at topology_path)
    using given generator (ti_simulation_generator_path)
    on given config (or use default of config_path is None).
    Puts generated simulation to output_file.
    Returns result of generator run
    """
    args = [
        "python3",
        ti_sumlation_generator_path,
        "-t",
        topology_path,
        "-o",
        output_file
    ]
    if config_path is not None:
        args += ["-c", config_path]
    return run_subprocess(args)
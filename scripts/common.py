import yaml
import sys
import subprocess
import os
from tempfile import TemporaryDirectory

SCRIPTS_DIR = os.path.dirname(__file__)

# Reads yaml from file by given path
def load_yaml(path : str) -> dict | None:
    """Loads yaml conig from given file"""
    try:
        with open(path, 'r') as f:
            return yaml.safe_load(f)
    except FileNotFoundError:
        print(f"Error: Configuration file '{path}' not found.")
        return None
    except yaml.YAMLError as e:
        print(f"Error: Invalid YAML in configuration file: {e}")
        return None

def save_yaml(data, filename):
    """Save data as YAML to a file"""
    with open(filename, "w") as f:
        yaml.Dumper.ignore_aliases = lambda _, __: True
        yaml.dump(data, f, sort_keys=False, default_flow_style=False, Dumper=yaml.Dumper)

def run_subprocess(subprocess_args : list[str], log_commands : bool = True, check_fails : bool = True):
    """
    Runs subprocess with given args. If command fails, prints its stderr to stderr
    Returns result of subprocess.
    """
    if log_commands:
        print(f"Run command: `{' '.join(subprocess_args)}`")

    result = subprocess.run(subprocess_args, capture_output=True, text=True)

    if result.returncode != 0 and check_fails:
        print(f"Command `{' '.join(subprocess_args)}` failed; stderr:", file=sys.stderr)
        print(result.stderr, file=sys.stderr)

    return result

def run_nons(
        nons_path : str,
        simulation_config_path : str,
        output_dir : str | None = None,
        metrics_filter : str | None = None,
        check_fails = True
        ):
    """
    Runs NoNs on with given config
    If simulation succeed, returns true
    Otherwice prints stderr of NoNS, simulation and topology configs
    """
    args = [
        nons_path,
        "-c",
        simulation_config_path,
        f"--output-dir={output_dir}"
    ]
    if metrics_filter is not None:
        args.append(f"--metrics-filter={metrics_filter}")
    return run_subprocess(args, check_fails)

def run_nons_in_temp_dir(nons_path : str,
        simulation_config_path : str, check_fails = True):
    with TemporaryDirectory() as tempdir:
        # run without metrics collection
        return run_nons(nons_path, simulation_config_path, tempdir, "", check_fails)
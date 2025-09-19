import yaml
import sys
import subprocess

# Reads yaml from file by given path
def load_yaml(path : str) -> dict:
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

def run_subprocess(subprocess_args : list[str], check_fails : bool = True):
    """
    Runs subprocess with given args
    Returns result of subprocess
    """
    result = subprocess.run(subprocess_args, capture_output=True, text=True)
    if result.returncode != 0 and check_fails:
        print(f"Command {subprocess_args} failed; stderr:", file=sys.stderr)
        print(result.stderr, file=sys.stderr)
    return result
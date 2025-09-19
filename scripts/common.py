import yaml
import sys

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
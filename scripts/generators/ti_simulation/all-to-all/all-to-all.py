import os

from generators.ti_simulation.common import *
from generators.common import *

def get_host_names(topology : dict) -> list[str]:
    try:
        return list(topology["hosts"].keys())
    except KeyError as e:
        raise RuntimeError(f"Topology config missing field {e}")
    
def generate_simulation_config(
        host_names : list[str],
        config : dict,
        topology_config_path : str) -> dict:
    
    try:
        flows_per_connection = config["flows_per_connection"]
        presets = config["presets"]
        try:
            connection_presets = presets["connection"]
            try:
                default_connection_preset = connection_presets["default"]
            except KeyError as e:
                raise RuntimeError(f"connection->{e}")
        
            flow_presets = presets["flow"]
            try:
                default_flow_preset = flow_presets["default"]
            except KeyError as e:
                raise RuntimeError(f"flow->{e}")
            
        except KeyError as e:
            raise RuntimeError(f"presets->{e}")    
    except KeyError as e:
        raise RuntimeError(f"Config missing field {e}")

    simulation_config = {
        "topology_config_path" : topology_config_path,
        "presets" : presets,
        "connections" : {}
    }

    for sender_id in host_names:
        for receiver_id in host_names:
            if sender_id == receiver_id:
                continue
            connection_name = f"conn_from_{sender_id}_to_{receiver_id}"

            connection = default_connection_preset|  {
                "sender_id": sender_id,
                "receiver_id": receiver_id,
                "flows" : {}
            }

            for flow_num in range(1, flows_per_connection + 1):
                connection["flows"][f"flow_{flow_num}"] = default_flow_preset
            
            simulation_config["connections"][connection_name] = connection 
    
    return simulation_config
def main():
    
    default_config_path = os.path.join(os.path.dirname(__file__), "default_config.yml")
    args = parse_ti_simulation_generator_args(default_config_path)

    config_path = args.config
    output_path = args.output_path

    config = load_yaml(config_path)
    topology_path = args.topology
    topology = load_yaml(topology_path)
    host_names = get_host_names(topology)

    output_dir = os.path.dirname(output_path)
    topology_path_rel_from_output = os.path.relpath(topology_path, output_dir)
    
    simulation_config = generate_simulation_config(host_names, config, topology_path_rel_from_output)

    save_yaml(simulation_config, output_path)
    print(f"Your simulation config saved to {output_path}")
    

if __name__ == "__main__":
    main()
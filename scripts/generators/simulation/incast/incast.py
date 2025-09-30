import os

from common import *
from generators.topology.common import *

# TODO: eliminate this class when all presets (switch, flow, connection) will be available
class InputConfig:
    def __init__(self, config : dict):
        # Gets required fields from config
        try:
            self.packet_spraying = config["packet-spraying"]
            self.senders_count = int(config["senders_count"])
            self.scenario = config["scenario"]
            
            presets = config["presets"]
            try:
                self.link_presets = presets["link"]
                switch_presets = presets["switch"]
                try:
                    self.default_switch_preset = switch_presets["default"]
                except KeyError as e:
                    raise KeyError(f"switch->{e}")
                
                connection_presets = presets["connection"]
                try:
                    self.default_connection_preset = connection_presets["default"]
                except KeyError as e:
                    raise KeyError(f"connection->{e}")
                
                flow_presets = presets["flow"]
                try:
                    self.default_flow_preset = flow_presets["default"].copy()
                except KeyError as e:
                    raise KeyError(f"flow->{e}")

            except KeyError as e:
                raise KeyError(f"presets->{e}")
        except KeyError as e:
            raise KeyError(f"Config missing value {e}")

class OutputConfig:
    def __init__(self):
        self.config = {}
        self.senders = []

    def _add_presets(self, presets : dict):
        self.config["presets"] = presets

    # private method
    def _add_topology(self, input_config : InputConfig):
        """
        Adds topology part and updated self.senders_list
        """
        self.config["packet-spraying"] = input_config.packet_spraying
        link_generator = LinkGenerator(self.config)

        switch_id = "switch"

        # Add senders
        self.config["hosts"] = {}
        for i in range(1, input_config.senders_count + 1):
            sender_id = f"sender{i}"
            self.senders.append(sender_id)
            self.config["hosts"][sender_id] = {}

            # Add links between sender and switch
            link_generator.add_bidirectional_link(sender_id, switch_id)   

        # Add switch
        # TODO: replace default_switch_preset with {} when switch presets will be available
        self.config["switches"] = {switch_id : input_config.default_switch_preset}

        # Add receiver
        receiver_id = "receiver"
        self.config["hosts"][receiver_id] = {}

        link_generator.add_bidirectional_link(switch_id, receiver_id) 
    
    def _add_simulation(self, input_config : InputConfig):
        """
        Adds simulation part
        Run it ONLY when self.senders are correct
        """
        self.config["connections"] = {}
        for i in range(len(self.senders)):
            # TODO: delete this line when connection presets will be available
            self.config["connections"][f"conn{i+1}"] = input_config.default_connection_preset | {
                "sender_id": self.senders[i],
                "receiver_id": "receiver",
                "flows": {
                    # TODO: replace config.default_flow_preset with {} when flow presets will be available
                    "flow": input_config.default_flow_preset
                },
            }

    def _add_scenario(self, input_config: InputConfig):
        """
        Adds scenario part
        """
        self.config["scenario"] = input_config.scenario

    def generate(self, config : dict) -> dict:
        """
        Generates incast config
        Contains both simulation & topology parts
        ATTENTION: value of 'topology_config_path' does not set!
        """
        self._add_presets(config["presets"])
        
        input_config = InputConfig(config)
        self._add_topology(input_config)
        self._add_simulation(input_config)
        self._add_scenario(input_config)
        return self.config
    
    def generate_and_save(self, config : dict, output_path : str):
        self.generate(config)
        output_filename = os.path.basename(output_path)
        self.config["topology_config_path"] = output_filename
        save_yaml(self.config, output_path)


def main():
    # Parse command line arguments
    args = parse_topology_generator_args(os.path.join(os.path.dirname(__file__), "default_config.yml"),
        "Generates incast topology & simulation (in one config).")
    
    output_path = args.output_path
    config = load_yaml(args.config)
    output_config = OutputConfig()

    output_config.generate_and_save(config, output_path)
    print(f"Incast config saved in {output_path}")


if __name__ == "__main__":
    main()

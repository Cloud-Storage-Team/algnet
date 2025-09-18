
class LinkGenerator:
    def __init__(self, topology : dict):
        self.next_link_num = 0
        self.topology = topology

    def add_directed_link(self, source_id : str, dest_id : str, preset_name : str = "default") -> int:
        """
        Adds to topology link from source_id to dest_id
        Link id is link_<next_link_num>
        """
        link_name = f"link_{self.next_link_num}"

        if "links" not in self.topology:
            self.topology["links"] = {}    
        
        self.topology["links"][link_name] = {
            "from" : source_id,
            "to" : dest_id
        }
        if preset_name != "default":
            self.topology["links"][link_name]["preset-name"] = preset_name
        self.next_link_num += 1

    def add_bidirectional_link(self,
                               first_device_id : str,
                               second_device_id : str,
                               forward_preset_name : str = "default",
                               backwards_preset_name : str = "default"):
        """
        Adds links two links between first_device_id and second_device_id:
        for link first->second used forward_preset_name
        for link second->first used backward_preset_name
        """
        next_link_num = self.add_directed_link(first_device_id, second_device_id, forward_preset_name)
        next_link_num = self.add_directed_link(second_device_id, first_device_id, backwards_preset_name)
        return next_link_num
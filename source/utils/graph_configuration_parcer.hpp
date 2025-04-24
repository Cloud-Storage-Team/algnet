#pragma once
#include <yaml-cpp/yaml.h>

#include <concepts>
#include <fstream>
#include <iostream>

#include "link.hpp"
#include "simulator.hpp"

namespace sim {

// returns throughput in Mbps
//  static uint32_t parse_throughtput(const std::string& throughput) {
//      size_t pos = throughput.find_first_of("0123456789");
//      if (pos == std::string::npos) {
//          throw std::invalid_argument("Invalid throughput format");
//      }

//     uint32_t value = std::stoi(throughput.substr(pos));
//     std::string unit = throughput.substr(pos +
//     std::to_string(value).length());

//     if (unit == "Gbps") {
//         return value * 1000;
//     } else if (unit == "Mbps") {
//         return value;
//     } else {
//         throw std::invalid_argument("Unsupported throughput unit");
//     }
// }

BasicSimulator parse_configuration(std::string path) {
    YAML::Node config = YAML::LoadFile(path);
    BasicSimulator simulator;
    if (!config) {
        std::cout << "Can not read yaml" << std::endl;
    }
    std::map<std::string, std::shared_ptr<IRoutingDevice> > hosts;

    for (const auto& host : config["hosts"]) {
        std::string name = host.first.as<std::string>();
        std::string type = host.second["type"].as<std::string>();

        std::shared_ptr<IRoutingDevice> ptr = nullptr;
        if (type == "sender") {
            ptr = simulator.add_sender(name);
        } else if (type == "receiver") {
            ptr = simulator.add_receiver(name);
        } else if (type == "switch") {
            ptr = simulator.add_switch(name);
        }
        if (ptr == nullptr) {
            throw std::runtime_error("Incorrect type of host: " + type);
        }
        hosts[name] = ptr;
        std::cout << "added device; type = " << type << " name = " << name
                  << std::endl;
    }

    for (const auto& links : config["links"]) {
        std::string name = links.first.as<std::string>();
        std::string from = links.second["from"].as<std::string>();
        std::string to = links.second["to"].as<std::string>();

        auto from_it = hosts.find(from);
        auto to_it = hosts.find(to);

        if (from_it == hosts.end()) {
            throw std::runtime_error("Can not add link from '" + from +
                                     "' to '" + to +
                                     "'; from device not exists");
        }
        if (to_it == hosts.end()) {
            throw std::runtime_error("Can not add link from '" + from +
                                     "' to '" + to + "'; to device not exists");
        }

        simulator.add_link(from_it->second, to_it->second, 0, 0);
        std::cout << "added link between " << from << " and " << to
                  << std::endl;
    }

    return simulator;
}

}  // namespace sim
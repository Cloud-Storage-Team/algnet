#include "parser.hpp"
#include <stdexcept>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

namespace sim {
	void YamlParser::parseConfig(const std::string &filename, Simulator &simulator) {
		YAML::Node config = YAML::LoadFile(filename);
		processHosts(config, simulator);
		processSwitches(config, simulator);
		processLinks(config, simulator);
		simulator.recalculate_paths();
	}

	uint32_t YamlParser::parseThroughput(const std::string &throughput_str) {
		size_t unit_pos = throughput_str.find_first_not_of("0123456789");
		if (unit_pos == std::string::npos) {
			throw std::runtime_error("Invalid throughput: " + throughput_str);
		}
		uint32_t value = std::stoul(throughput_str.substr(0, unit_pos));
		std::string unit = throughput_str.substr(unit_pos);
		if (unit == "Gbps") return value * 1000; // Convert to Mbps
		if (unit == "Mbps") return value;
		throw std::runtime_error("Unsupported throughput unit: " + unit);
	}

	uint32_t YamlParser::parseLatency(const std::string &latency_str) {
		size_t unit_pos = latency_str.find_first_not_of("0123456789");
		if (unit_pos == std::string::npos) {
			throw std::runtime_error("Invalid latency: " + latency_str);
		}
		uint32_t value = std::stoul(latency_str.substr(0, unit_pos));
		std::string unit = latency_str.substr(unit_pos);
		if (unit == "ms") return value;
		throw std::runtime_error("Unsupported latency unit: " + unit);
	}

	void YamlParser::processHosts(const YAML::Node &config, Simulator &simulator) {
		if (!config["hosts"]) return;
		for (const auto &node: config["hosts"]) {
			std::string key = node.first.as<std::string>();
			auto host = node.second;
			std::string type_str = host["type"].as<std::string>();
			std::string name = host["name"].as<std::string>();

			DeviceType type;
			if (type_str == "sender") type = DeviceType::SENDER;
			else if (type_str == "receiver") type = DeviceType::RECEIVER;
			else throw std::runtime_error("Invalid host type: " + type_str);

			devices_map[key] = simulator.add_device(name, type);
		}
	}

	void YamlParser::processSwitches(const YAML::Node &config, Simulator &simulator) {
		if (!config["switches"]) return;
		for (const auto &node: config["switches"]) {
			std::string key = node.first.as<std::string>();
			auto sw = node.second;
			std::string name = sw["name"].as<std::string>();
			devices_map[key] = simulator.add_device(name, DeviceType::SWITCH);
		}
	}

	void YamlParser::processLinks(const YAML::Node &config, Simulator &simulator) {
		if (!config["links"]) return;
		for (const auto &node: config["links"]) {
			auto link = node.second;
			std::string from = link["from"].as<std::string>();
			std::string to = link["to"].as<std::string>();
			uint32_t latency = parseLatency(link["latency"].as<std::string>());
			uint32_t speed = parseThroughput(link["throughput"].as<std::string>());
			simulator.add_link(devices_map.at(from), devices_map.at(to), speed, latency);
		}
	}
} // namespace sim

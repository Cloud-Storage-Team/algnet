#pragma once

#include "../source/simulator.hpp"
#include <map>
#include <string>
#include <cstdint>
#include <yaml-cpp/node/node.h>


namespace sim {

	class YamlParser {
	public:
		void parseConfig(const std::string& filename, Simulator& simulator);

	private:
		std::map<std::string, IRoutingDevice*> devices_map;

		static uint32_t parseThroughput(const std::string& throughput_str);
		static uint32_t parseLatency(const std::string& latency_str);

		void processHosts(const YAML::Node& config, Simulator& simulator);
		void processSwitches(const YAML::Node& config, Simulator& simulator);
		void processLinks(const YAML::Node& config, Simulator& simulator);
	};

} // namespace sim
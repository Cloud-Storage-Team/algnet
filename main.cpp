#include <iostream>
#include "logger/logger.hpp"

#include "source/simulator.hpp"
#include "parser.hpp"


#include <yaml-cpp/yaml.h>

int main(const int argc, char **argv) {
	LOG_INFO("Hello, World!");
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config.yaml>\n";
		return 1;
	}

	try {
		sim::YamlParser parser;
		sim::BasicSimulator simulator = parser.
				parseConfig(argv[1]);
		simulator.start(1000); // Run simulation for 1000 time units
	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

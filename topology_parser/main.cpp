#include "parser.hpp"

#include <iostream>

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config.yaml>\n";
		return 1;
	}

	try {
		sim::Simulator simulator;
		sim::YamlParser parser;
		parser.parseConfig(argv[1], simulator);
		simulator.start(1000); // Run simulation for 1000 time units
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}
	return 0;
}
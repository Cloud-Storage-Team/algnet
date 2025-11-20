#include "ecn_parser.hpp"

namespace sim {

ECN EcnParser::parse_ecn(const ConfigNode& node) {
    float min = node["min"].value_or_throw().as_or_throw<float>();
    float max = node["max"].value_or_throw().as_or_throw<float>();
    float probability =
        node["probability"].value_or_throw().as_or_throw<float>();
    return ECN(min, max, probability);
}

}
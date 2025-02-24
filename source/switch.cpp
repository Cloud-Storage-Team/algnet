#include "switch.hpp"

#include "link.h"

namespace sim {

Switch::Switch(DeviceType a_type) : Device(a_type) {}

void Switch::process() { Link* link = next_inlink(); }

}  // namespace sim

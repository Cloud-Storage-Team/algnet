#include "dcqcn_parser.hpp"

namespace sim {
DCQCN parse_dcqcn([[maybe_unused]] const ConfigNodeWithPreset& node) {
    // TODO: implement correctlty
    ParamsDQCCN params;
    return DCQCN(params);
}
}  // namespace sim
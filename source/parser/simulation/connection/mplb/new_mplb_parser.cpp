#include "new_mplb_parser.hpp"

#include "single_cc_mplb_parser.hpp"

#include "new_i_mocks/new_conn_mock.hpp"
#include "new_i_mocks/new_mplb_mock.hpp"
#include "parser/parse_utils.hpp"

namespace sim {

std::shared_ptr<INewMPLB> NewMplbParser::parse_i_mplb(
    const ConfigNode& node, std::shared_ptr<IHost> sender,
    std::shared_ptr<IHost> receiver) {
    std::string mplb_type =
        node["type"].value_or_throw().as_or_throw<std::string>();
    if (mplb_type == "single-cc") {
        return SingleCCMplbParser::parse(node, sender, receiver);
    } else {
        throw std::runtime_error("Unknown MPLB type: " + mplb_type);
    }
}

}  // namespace sim

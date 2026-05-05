#include "dcqcn_parser.hpp"
#include "parser/parse_utils.hpp"

namespace sim {
DCQCN parse_dcqcn(const ConfigNodeWithPreset& node) {
    ParamsDQCCN params;

    if (auto start_rate_node = node["start_target_rate"]) {
        params.start_target_rate = parse_speed(start_rate_node->get_node());
    }

    // -------- Rate increment --------
    if (auto rpg_time_reset_node = node["rpg_time_reset"]) {
        params.rpg_time_reset = parse_time(rpg_time_reset_node->get_node());
    }

    if (auto rpg_byte_reset_node = node["rpg_byte_reset"]) {
        params.rpg_byte_reset = parse_size(rpg_byte_reset_node->get_node());
    }

    if (auto rpg_threshold_node = node["rpg_threshold"]) {
        params.rpg_threshold = rpg_threshold_node->as_or_throw<std::size_t>();
    }

    if (auto rpg_ai_rate_node = node["rpg_ai_rate"]) {
        params.rpg_ai_rate = parse_speed(rpg_ai_rate_node->get_node());
    }

    if (auto rpg_hai_rate_node = node["rpg_hai_rate"]) {
        params.rpg_hai_rate = parse_speed(rpg_hai_rate_node->get_node());
    }

    // -------- Alpha update --------
    if (auto initial_alpha_node = node["initial_alpha_value"]) {
        params.initial_alpha_value = initial_alpha_node->as_or_throw<int>();
    }

    if (auto dce_tcp_g_node = node["dce_tcp_g"]) {
        params.dce_tcp_g = dce_tcp_g_node->as_or_throw<int>();
    }

    if (auto dce_tcp_rtt_node = node["dce_tcp_rtt"]) {
        params.dce_tcp_rtt = parse_time(dce_tcp_rtt_node->get_node());
    }

    // -------- Rate decrement --------
    if (auto rate_reduce_period_node = node["rate_reduce_monitor_period"]) {
        params.rate_reduce_monitor_period = parse_time(rate_reduce_period_node->get_node());
    }

    if (auto first_cnp_rate_node = node["rate_to_set_on_first_cnp"]) {
        params.rate_to_set_on_first_cnp = parse_speed(first_cnp_rate_node->get_node());
    }

    if (auto clamp_tgt_rate_node = node["clamp_tgt_rate"]) {
        params.clamp_tgt_rate = clamp_tgt_rate_node->as_or_throw<bool>();
    }

    if (auto rpg_gd_node = node["rpg_gd"]) {
        params.rpg_gd = rpg_gd_node->as_or_throw<int>();
    }

    if (auto rpg_min_rate_node = node["rpg_min_rate"]) {
        params.rpg_min_rate = parse_speed(rpg_min_rate_node->get_node());
    }

    if (auto rpg_min_dec_fac_node = node["rpg_min_dec_fac"]) {
        params.rpg_min_dec_fac = rpg_min_dec_fac_node->as_or_throw<double>();
    }

    return DCQCN(params);
}
}  // namespace sim

#include "parser/parse_utils.hpp"

#include <spdlog/fmt/fmt.h>

#include <charconv>
#include <stdexcept>

namespace sim {

static utils::StrExpected<std::pair<uint32_t, std::string> > parse_value_unit(
    const std::string &value_with_unit) {
    const size_t unit_pos = value_with_unit.find_first_not_of("0123456789");
    if (unit_pos == std::string::npos) {
        return std::unexpected("Can not find unit : " + value_with_unit);
    }
    std::string value_str = value_with_unit.substr(0, unit_pos);
    uint32_t value;
    std::errc from_chars_err =
        std::from_chars(value_str.data(), value_str.data() + value_str.size(),
                        value)
            .ec;
    if (from_chars_err == std::errc()) {
        // no error
        const std::string unit = value_with_unit.substr(unit_pos);
        return std::make_pair(value, unit);
    }
    if (from_chars_err == std::errc::invalid_argument) {
        return std::unexpected(
            fmt::format("Can not convert {} to integer", std::move(value_str)));
    }
    if (from_chars_err == std::errc::result_out_of_range) {
        return std::unexpected(
            fmt::format("{} can not be represented as integer: out of range",
                        std::move(value_str)));
    }
    return std::unexpected(
        fmt::format("Unexpected error from std::from_chars"));
}

utils::StrExpected<SpeedGbps> parse_speed(const std::string &throughput) {
    auto maybe_value_unit = parse_value_unit(throughput);

    if (!maybe_value_unit) {
        return std::unexpected(maybe_value_unit.error());
    }

    auto [value, unit] = maybe_value_unit.value();
    if (unit == "Mbps") {
        return SpeedGbps(Speed<MBit, Second>(value));
    }
    if (unit == "Gbps") {
        return SpeedGbps(value);
    }
    return std::unexpected("Unsupported throughput unit: " + unit);
}

utils::StrExpected<TimeNs> parse_time(const std::string &time) {
    auto maybe_value_unit = parse_value_unit(time);

    if (!maybe_value_unit) {
        return std::unexpected(maybe_value_unit.error());
    }

    auto [value, unit] = maybe_value_unit.value();
    if (unit == "ns") {
        return TimeNs(value);
    }
    if (unit == "us") {
        return TimeNs(Time<Microsecond>(value));
    }
    if (unit == "ms") {
        return TimeNs(Time<Millisecond>(value));
    }
    if (unit == "s") {
        return TimeNs(Time<Second>(value));
    }
    return std::unexpected("Unsupported time unit: " + unit);
}

utils::StrExpected<SizeByte> parse_size(const std::string &size) {
    auto maybe_value_unit = parse_value_unit(size);

    if (!maybe_value_unit) {
        return std::unexpected(maybe_value_unit.error());
    }

    auto [value, unit] = maybe_value_unit.value();
    if (unit == "b") {
        return SizeByte(Size<Bit>(value));
    }
    if (unit == "B") {
        return SizeByte(value);
    }
    if (unit == "Kb") {
        return SizeByte(Size<KBit>(value));
    }
    if (unit == "KB") {
        return SizeByte(Size<KByte>(value));
    }
    if (unit == "Mb") {
        return SizeByte(Size<MBit>(value));
    }
    if (unit == "MB") {
        return SizeByte(Size<MByte>(value));
    }
    if (unit == "Gb") {
        return SizeByte(Size<GBit>(value));
    }
    if (unit == "GB") {
        return SizeByte(Size<GByte>(value));
    }
    return std::unexpected("Unsupported size unit: " + unit);
}

SpeedGbps parse_speed(const ConfigNode &throughput_node) {
    std::string throughput = throughput_node.as_or_throw<std::string>();
    utils::StrExpected<SpeedGbps> result = parse_speed(throughput);
    if (!result) {
        throw throughput_node.create_parsing_error(result.error());
    }
    return result.value();
}

TimeNs parse_time(const ConfigNode &time_node) {
    std::string time = time_node.as_or_throw<std::string>();
    utils::StrExpected<TimeNs> result = parse_time(time);
    if (!result) {
        throw time_node.create_parsing_error(result.error());
    }
    return result.value();
}

SizeByte parse_size(const ConfigNode &size_node) {
    std::string size = size_node.as_or_throw<std::string>();
    utils::StrExpected<SizeByte> result = parse_size(size);
    if (!result) {
        throw size_node.create_parsing_error(result.error());
    }
    return result.value();
}

std::regex parse_regex(const ConfigNode &regex_node) {
    try {
        return std::regex(regex_node.as_or_throw<std::string>());
    } catch (const std::regex_error &e) {
        throw regex_node.create_parsing_error("Can not construct regex: " +
                                              std::string(e.what()));
    }
}

}  // namespace sim
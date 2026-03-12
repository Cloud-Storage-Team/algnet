#pragma once

#include <spdlog/fmt/bundled/format.h>

#include <cstdlib>

#include "default_formatter.hpp"
#include "speed.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase, typename Char>
struct fmt::formatter<Speed<TSizeBase, TTimeBase>, Char>
    : DefaultFormatter<double, Char> {
    template <typename FormatContext>
    auto format(const Speed<TSizeBase, TTimeBase>& t,
                FormatContext& ctx) const {
        auto out = ctx.out();
        if (this->is_empty_format) {
            out = fmt::format_to(out, this->default_precision_format_str,
                                 t.value());
        } else {
            out = fmt::formatter<double, Char>::format(t.value(), ctx);
        }
        for (auto c : Speed<TSizeBase, TTimeBase>::get_suffix()) {
            *out++ = static_cast<Char>(c);
        }
        return out;
    }

private:
    static constexpr std::string_view default_precision_format_str = "{:.1f}";
};

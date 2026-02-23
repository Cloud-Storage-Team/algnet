#pragma once

#include <spdlog/fmt/bundled/format.h>
#include <cstdlib>

#include "time.hpp"
#include "default_formatter.hpp"

template <IsTimeBase TTimeBase, typename Char>
struct fmt::formatter<Time<TTimeBase>, Char> : DefaultFormatter<double, Char> {

    template <typename FormatContext>
    auto format(const Time<TTimeBase>& t, FormatContext& ctx) const {
        auto out = ctx.out();
        if (this->is_empty_format) {
            out = fmt::format_to(out, default_precision_format_str, t.value());
        } else {
            out = fmt::formatter<double, Char>::format(t.value(), ctx);
        }
        for (auto c : TTimeBase::suffix) {
            *out++ = static_cast<Char>(c);
        }
        return out;
    }
private:
    static constexpr std::string_view default_precision_format_str = "{:.1f}";
};
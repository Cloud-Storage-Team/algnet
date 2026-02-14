#pragma once

#include <spdlog/fmt/bundled/format.h>
#include <cstdlib>

#include "time.hpp"
#include "common_formatter.hpp"

template <IsTimeBase TTimeBase, typename Char>
struct fmt::formatter<Time<TTimeBase>, Char> : CommonFormatter<Char> {

    template <typename FormatContext>
    auto format(const Time<TTimeBase>& t, FormatContext& ctx) const {
        auto out = ctx.out();
        if (this->use_default_precision) {
            out = fmt::format_to(out, this->default_precision_format_str, t.value());
        } else {
            out = fmt::formatter<double, Char>::format(t.value(), ctx);
        }
        for (auto c : TTimeBase::suffix) {
            *out++ = static_cast<Char>(c);
        }
        return out;
    }
private:
    static constexpr const char* default_precision_format_str = "{:.1f}";
};
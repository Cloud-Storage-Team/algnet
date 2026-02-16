#pragma once

#include <spdlog/fmt/bundled/format.h>
#include <cstdlib>

#include "speed.hpp"
#include "common_formatter.hpp"

template <IsSizeBase TSizeBase, IsTimeBase TTimeBase, typename Char>
struct fmt::formatter<Speed<TSizeBase, TTimeBase>, Char> : CommonFormatter<Char>{

    template <typename FormatContext>
    auto format(const Speed<TSizeBase, TTimeBase>& t, FormatContext& ctx) const {
        auto out = ctx.out();
        if (this->use_default_precision) {
            out = fmt::format_to(out, this->default_precision_format_str, t.value());
        } else {
            out = fmt::formatter<double, Char>::format(t.value(), ctx);
        }
        for (auto c : TSizeBase::suffix) {
            *out++ = static_cast<Char>(c);
        }
        auto sep = Speed<TSizeBase, TTimeBase>::m_separator;
        out = std::copy(sep.begin(), sep.end(), out);
        for (auto c: TTimeBase::suffix){
            *out++ = static_cast<Char>(c);
        }
        return out;
    }
private:
    static constexpr const char* default_precision_format_str = "{:.1f}";
};
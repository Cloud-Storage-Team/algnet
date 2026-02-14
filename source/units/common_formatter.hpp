#pragma once

#include <spdlog/fmt/bundled/format.h>

template<typename Char>
struct CommonFormatter: fmt::formatter<double, Char>{

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end() || *it == '}') {
            // empty format string; use default precision;
            use_default_precision = true;
            return it;
        }
        use_default_precision = false;
        return fmt::formatter<double, Char>::parse(ctx);
    }

protected:
    bool use_default_precision = false;
};
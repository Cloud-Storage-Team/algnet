#pragma once

#include <spdlog/fmt/bundled/format.h>

template<fmt::formattable<char> T, typename Char>
struct DefaultFormatter: fmt::formatter<T, Char>{

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end() || *it == '}') {
            // empty format string;
            is_empty_format = true;
            return it;
        }
        is_empty_format = false;
        return fmt::formatter<T, Char>::parse(ctx);
    }

protected:
    bool is_empty_format = false;
};
#pragma once

#include <spdlog/fmt/bundled/format.h>
#include <cstdlib>

#include "size.hpp"

template <IsSizeBase TSizeBase, typename Char>
struct fmt::formatter<Size<TSizeBase>, Char>
    : fmt::formatter<uint64_t, Char>
{
    using Base = fmt::formatter<uint64_t, Char>;

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return Base::parse(ctx);
    }

    template <typename FormatContext>
    auto format(const Size<TSizeBase>& t, FormatContext& ctx) const {
        auto out = Base::format(t.value(), ctx);

        for (auto c : TSizeBase::suffix) {
            *out++ = static_cast<Char>(c);
        }

        return out;
    }
};
#pragma once

#include <spdlog/fmt/bundled/format.h>
#include <cstdlib>

#include "size.hpp"
#include "default_formatter.hpp"

template <IsSizeBase TSizeBase, typename Char>
struct fmt::formatter<Size<TSizeBase>, Char> : DefaultFormatter<uint64_t, Char> {

    template <typename FormatContext>
    auto format(const Size<TSizeBase>& t, FormatContext& ctx) const {
        auto out = ctx.out();
        if (this->is_empty_format) {
            out = fmt::format_to(out, default_precision_format_str, t.value());
        } else {
            out = fmt::formatter<uint64_t, Char>::format(t.value(), ctx);
        }
        for (auto c : TSizeBase::suffix) {
            *out++ = static_cast<Char>(c);
        }
        return out;
    }

private:
    static constexpr std::string_view default_precision_format_str = "{}";
};
#include <functional>
#include <iostream>
#include <tuple>
#include <unordered_map>

template <typename... Args> std::size_t generateKey(Args... args) {
    std::size_t seed = 0;
    static std::unordered_map<std::size_t, bool> seedMap;

    auto hashCombine = [&seed](const auto &arg) {
        using ArgType = std::decay_t<decltype(arg)>;
        std::hash<ArgType> hasher;
        seed ^= hasher(arg) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };

    (hashCombine(args), ...);

    return seed;
}
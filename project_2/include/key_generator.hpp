#include <string>
#include <sstream>
#include <tuple>

template<typename... Args>
std::string generateKey(Args... args) {
    std::ostringstream oss;
    auto tuple = std::make_tuple(args...);
    std::apply([&oss](const auto&... arg) {
        bool first = true;
        ((oss << (first ? "" : "_") << arg, first = false), ...);
    }, tuple);
    return oss.str();
}
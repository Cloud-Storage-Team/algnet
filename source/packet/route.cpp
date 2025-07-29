#include "route.hpp"

#include <spdlog/fmt/fmt.h>

namespace sim {

Route::Route(Endpoint a_source, Endpoint a_dest)
    : source(std::move(a_source)), dest(std::move(a_dest)) {}

Route::Route(Id source_id, Id dest_id)
    : Route(Endpoint(std::move(source_id)), Endpoint(std::move(dest_id))) {}

bool Route::operator==(const Route& route) const {
    return source == route.source && dest == route.dest;
}

std::string Route::to_string() const {
    return fmt::format("{} -> {}", source.to_string(), dest.to_string());
}

std::ostream& operator<<(std::ostream& out, const Route& route) {
    return out << route.to_string();
}

}  // namespace sim
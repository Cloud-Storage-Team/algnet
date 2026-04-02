#include "common.hpp"

#include "link/link_parser.hpp"

namespace sim {

void add_directed_link(std::shared_ptr<IDevice> from,
                       std::shared_ptr<IDevice> to,
                       const ConfigNodeWithPreset& preset,
                       utils::IdTable<ILink>& links_table) {
    std::shared_ptr<ILink> link = parse_i_link(from, to, preset);
    if (!links_table.emplace(link->get_id(), link).second) {
        throw std::runtime_error(
            fmt::format("Duplicate of link with name {} ", link->get_id()));
    }
};

void add_links_between(std::shared_ptr<IDevice> from,
                       std::shared_ptr<IDevice> to,
                       const ConfigNodeWithPreset& preset,
                       utils::IdTable<ILink>& links_table) {
    add_directed_link(from, to, preset, links_table);
    add_directed_link(to, from, preset, links_table);
};

}  // namespace sim
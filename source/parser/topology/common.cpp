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

void add_directed_link(std::shared_ptr<IDevice> from,
                       std::shared_ptr<IDevice> to,
                       const ConfigNodeWithPreset& preset,
                       Storage<ILink>& links) {
    links.emplace_back(parse_i_link(from, to, preset));
}

void add_links_between(std::shared_ptr<IDevice> device_1,
                       std::shared_ptr<IDevice> device_2,
                       const ConfigNodeWithPreset& preset,
                       Storage<ILink>& links) {
    add_directed_link(device_1, device_2, preset, links);
    add_directed_link(device_2, device_1, preset, links);
}

}  // namespace sim
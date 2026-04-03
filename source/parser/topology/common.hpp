#pragma once
#include "parser/config_reader/config_node_with_preset_.hpp"
#include "topology/link/i_link.hpp"
#include "utils/id_table.hpp"

namespace sim {

template <typename T>
using Storage = std::vector<std::shared_ptr<T> >;

void add_directed_link(std::shared_ptr<IDevice> from,
                       std::shared_ptr<IDevice> to,
                       const ConfigNodeWithPreset& preset,
                       utils::IdTable<ILink>& links_table);

void add_links_between(std::shared_ptr<IDevice> device_1,
                       std::shared_ptr<IDevice> device_2,
                       const ConfigNodeWithPreset& preset,
                       utils::IdTable<ILink>& links_table);

void add_directed_link(std::shared_ptr<IDevice> from,
                       std::shared_ptr<IDevice> to,
                       const ConfigNodeWithPreset& preset,
                       Storage<ILink>& links);

void add_links_between(std::shared_ptr<IDevice> device_1,
                       std::shared_ptr<IDevice> device_2,
                       const ConfigNodeWithPreset& preset,
                       Storage<ILink>& links);

}  // namespace sim

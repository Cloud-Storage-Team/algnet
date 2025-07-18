#include "links_queue_size_storage.hpp"

#include "draw_plots.hpp"
#include "link/i_link.hpp"
#include "multi_id_metrics_storage.hpp"
#include "utils/safe_matplot.hpp"
#include "write_to_csv.hpp"

namespace sim {

LinksQueueSizeStorage::LinksQueueSizeStorage(std::string a_filter)
    : m_filter(a_filter) {}

void LinksQueueSizeStorage::add_record(Id id, LinkQueueType type, Time time,
                                       double value) {
    std::pair<Id, LinkQueueType> key = std::make_pair(id, type);
    auto it = m_storage.find(key);
    if (it == m_storage.end()) {
        std::string filename = get_metrics_filename(id);
        if (!std::regex_match(filename, m_filter)) {
            m_storage[std::move(key)] = std::nullopt;
        } else {
            MetricsStorage new_storage;
            new_storage.add_record(time, value);
            m_storage.emplace(std::move(key), std::move(new_storage));
        }
    } else if (it->second.has_value()) {
        it->second->add_record(time, value);
    }
}

void LinksQueueSizeStorage::export_to_files(
    std::filesystem::path output_dir_path) const {
    std::map<Id, std::vector<std::pair<MetricsStorage, std::string> > >
        multi_id_storage;
    for (const auto& [key, values] : data()) {
        auto [id, type] = key;
        multi_id_storage[id].emplace_back(values, to_string(type));
    }
    for (auto [id, storages] : multi_id_storage) {
        write_to_csv(storages, output_dir_path / get_metrics_filename(id));
    }
}

void LinksQueueSizeStorage::draw_plots(
    std::filesystem::path output_dir_path) const {
    // for data from both from_ingress and to_ingress queue sizes
    std::map<Id, PlotMetricsData> queue_size_data;
    for (auto& [key, values] : data()) {
        auto [link_id, type] = key;
        std::string curve_name = to_string(type);
        std::replace(curve_name.begin(), curve_name.end(), '_', ' ');
        queue_size_data[link_id].emplace_back(values, curve_name);
    }
    for (auto [link_id, data] : queue_size_data) {
        auto link =
            IdentifierFactory::get_instance().get_object<ILink>(link_id);
        PlotMetadata metadata = {
            "Time, ns", "Values, bytes",
            fmt::format("Queue size from {} to {}", link->get_from()->get_id(),
                        link->get_to()->get_id())};

        auto fig = put_on_same_plot(data, metadata);
        auto ax = fig->current_axes();

        auto limits = ax->xlim();

        auto draw_gorizontal_line = [&limits](
                                        double line_y, std::string_view name,
                                        std::initializer_list<float> color) {
            matplot::line(0, line_y, limits[1], line_y)
                ->line_width(1.5)
                .color(color)
                .display_name(name);
        };

        draw_gorizontal_line(link->get_max_from_egress_buffer_size(),
                             "max from egress", {1.f, 0.f, 0.f});
        draw_gorizontal_line(link->get_max_to_ingress_queue_size(),
                             "max to ingress", {0.f, 0.f, 1.f});

        ax->xlim({0, limits[1]});
        ax->color("white");

        std::filesystem::path plot_path =
            output_dir_path / fmt::format("{}.svg", link_id);

        matplot::safe_save(fig, plot_path.string());
    }
}

std::map<std::pair<Id, LinkQueueType>, MetricsStorage>
LinksQueueSizeStorage::data() const {
    std::map<std::pair<Id, LinkQueueType>, MetricsStorage> result;
    for (auto [id, maybe_storage] : m_storage) {
        if (maybe_storage) {
            result[id] = maybe_storage.value();
        }
    }
    return result;
}

std::string LinksQueueSizeStorage::get_metrics_filename(Id id) const {
    return fmt::format("queue_size/{}.csv", id);
}
}  // namespace sim
#pragma once
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <optional>
#include <type_traits>

#include "utils/str_expected.hpp"

namespace sim {

class ConfigNode;

using ConfigNodeExpected = utils::StrExpected<ConfigNode>;

class ConfigNode {
public:
    explicit ConfigNode(YAML::Node a_node = YAML::Node(YAML::NodeType::Null),
                        std::optional<std::string> a_name = std::nullopt);

    // Some functional over yaml-cpp

    [[nodiscard]] const YAML::Node& get_node() const noexcept;

    [[nodiscard]] const std::optional<std::string>& get_name() const noexcept;

    [[nodiscard]] const std::string& get_name_or_throw() const;

    [[nodiscard]] std::runtime_error create_parsing_error(
        std::string_view error) const;

    friend std::ostream& operator<<(std::ostream& out, const ConfigNode& node);

    // yaml-cpp functional

    [[nodiscard]] YAML::NodeType::value Type() const;
    [[nodiscard]] bool IsNull() const noexcept;
    [[nodiscard]] bool IsScalar() const noexcept;
    [[nodiscard]] bool IsSequence() const noexcept;
    [[nodiscard]] bool IsMap() const noexcept;

    // access
    template <typename T>
    [[nodiscard]] utils::StrExpected<T> as() const noexcept {
        try {
            return m_node.as<T>();
        } catch (const std::exception& e) {
            return std::unexpected(e.what());
        } catch (...) {
            return std::unexpected(
                "Undefined exception while calling `YAML::Node::as`");
        }
    }

    template <typename T>
    T as_or_throw() const {
        return as<T>().value_or_throw();
    }

    [[nodiscard]] const std::string& Tag() const noexcept;

    [[nodiscard]] std::size_t size() const noexcept;

    class Iterator {
    public:
        Iterator(YAML::const_iterator a_it);

        Iterator& operator++();

        Iterator operator++(int);

        bool operator==(const Iterator& rhs) const;

        bool operator!=(const Iterator& rhs) const;

        ConfigNode operator*() const;

    private:
        // Invariant: m_stacktrace_node is not null
        YAML::const_iterator m_iterator;
    };

    Iterator begin() const;
    Iterator end() const;

    // indexing
    ConfigNodeExpected operator[](std::string_view key) const;

private:
    // Always correct
    const YAML::Node m_node;

    const std::optional<std::string> m_name;
};

ConfigNode load_file(std::filesystem::path path);

}  // namespace sim

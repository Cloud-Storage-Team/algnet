#pragma once
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "utils/str_expected.hpp"

namespace sim {

class ConfigNode;

using ConfigNodeExpected = utils::StrExpected<ConfigNode>;

class ConfigNode {
public:
    // Creates ROOT ConfigNode from YAML::Node
    explicit ConfigNode(YAML::Node a_node = YAML::Node(),
                        std::optional<std::string> a_name = std::nullopt);

    // Some functional over yaml-cpp

    const YAML::Node& get_node() const;

    const std::optional<std::string>& get_name() const;

    const std::string& get_name_or_throw() const;

    std::runtime_error create_parsing_error(std::string_view error) const;

    // yaml-cpp functional

    YAML::NodeType::value Type() const;
    bool IsNull() const;
    bool IsScalar() const;
    bool IsSequence() const;
    bool IsMap() const;

    // access
    template <typename T>
    utils::StrExpected<T> as() const {
        try {
            return m_node.as<T>();
        } catch (const YAML::Exception& e) {
            return std::unexpected(e.what());
        }
    }

    template <typename T>
    T as_or_throw() const {
        return as<T>().value_or_throw();
    }

    const std::string& Scalar() const;

    const std::string& Tag() const;

    std::size_t size() const;

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

    friend std::ostream& operator<<(std::ostream& out, const ConfigNode& node);

private:
    // Invariant: m_node is correct, m_stacktrace_node is not null
    const YAML::Node m_node;
    const std::optional<std::string> m_name;
};

ConfigNode load_file(std::filesystem::path path);

}  // namespace sim

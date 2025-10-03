#pragma once
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "node_stacktrace.hpp"

namespace sim {

class ConfigNodeError : public std::runtime_error {
public:
    ConfigNodeError(std::string a_what);
};

class ConfigNode {
public:
    // Creates ROOT ConfigNode from YAML::Node
    explicit ConfigNode(YAML::Node a_node = YAML::Node());

    // Some functional over yaml-cpp

    std::optional<std::string> get_name() const;

    const YAML::Node& get_node() const;

    // yaml-cpp functional

    YAML::NodeType::value Type() const;
    bool IsDefined() const;
    bool IsNull() const;
    bool IsScalar() const;
    bool IsSequence() const;
    bool IsMap() const;

    // bool conversions
    explicit operator bool() const;
    bool operator!() const;

    // access
    template <typename T>
    T as() const {
        try {
            return m_node.as<T>();
        } catch (const YAML::Exception& e) {
            throw ConfigNodeError("Can not apply 'as' to node\n" +
                                  m_stacktrace_node->to_string() +
                                  "\n; original error: " + e.what());
        }
    }
    template <typename T, typename S>
    T as(const S& fallback) const {
        try {
            return m_node.as<T>(fallback);
        } catch (const YAML::Exception& e) {
            throw ConfigNodeError("Can not apply 'as' with fallback to node\n" +
                                  m_stacktrace_node->to_string() +
                                  "\n; original error: " + e.what());
        }
    }
    const std::string& Scalar() const;

    const std::string& Tag() const;

    std::size_t size() const;

    class Iterator {
    public:
        Iterator(YAML::const_iterator a_it, NodeStacktracePtr a_stacktrace);

        Iterator& operator++();

        Iterator operator++(int);

        bool operator==(const Iterator& rhs) const;

        bool operator!=(const Iterator& rhs) const;

        ConfigNode operator*() const;

    private:
        // Invariant: m_stacktrace_node is not null
        YAML::const_iterator m_iterator;
        NodeStacktracePtr m_stacktrace_node;
    };

    Iterator begin() const;
    Iterator end() const;

    // indexing
    const ConfigNode operator[](std::string_view key) const;

private:
    ConfigNode(YAML::Node a_node, NodeStacktracePtr a_node_ptr);

    // Invariant: m_node is correct, m_stacktrace_node is not null
    const YAML::Node m_node;
    NodeStacktracePtr m_stacktrace_node;
};

std::ostream& operator<<(std::ostream& out, const ConfigNode& node);

bool operator==(const ConfigNode& lhs, const ConfigNode& rhs);

ConfigNode load_file(std::filesystem::path path);

}  // namespace sim

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
#include "utils/str_expected.hpp"

namespace sim {

class ConfigNodeError : public std::runtime_error {
public:
    ConfigNodeError(std::string a_what);
};

class ConfigNode;

using ConfigNodeExpected = utils::StrExpected<ConfigNode>;

class ConfigNode {
public:
    // Creates ROOT ConfigNode from YAML::Node
    explicit ConfigNode(YAML::Node a_node = YAML::Node());

    // Some functional over yaml-cpp

    std::optional<std::string> get_name() const;

    const YAML::Node& get_node() const;

    NodeStacktracePtr get_stacktrace() const;

    ConfigNodeError create_parsing_error(std::string_view error) const;

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
            return std::unexpected("Can not apply 'as' to node\n" +
                                   m_stacktrace_node->to_string() +
                                   "\n; original error: " + e.what());
        }
    }

    template <typename T>
    T as_or_throw() const {
        return as<T>().value_or_throw();
    }

    template <typename T, typename S>
    utils::StrExpected<T> as(const S& fallback) const {
        try {
            return m_node.as<T>(fallback);
        } catch (const YAML::Exception& e) {
            return std::unexpected(
                "Can not apply 'as' with fallback to node\n" +
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

    ConfigNodeExpected operator[](std::string_view key) const;

private:
    ConfigNode(YAML::Node a_node, NodeStacktracePtr a_node_ptr);

    // Invariant: m_node is correct, m_stacktrace_node is not null
    const YAML::Node m_node;
    NodeStacktracePtr m_stacktrace_node;
};

ConfigNode load_file(std::filesystem::path path);

}  // namespace sim

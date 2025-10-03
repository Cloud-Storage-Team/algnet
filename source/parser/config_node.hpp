#pragma once
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <vector>

namespace sim {

class ConfigNodeError : public std::runtime_error {
public:
    ConfigNodeError(std::string a_what);
};

// Path by node names from root to current node
using NodeStacktrace = std::vector<std::string>;

std::ostream& operator<<(std::ostream& out, const NodeStacktrace& stacktrace);

std::string to_string(const NodeStacktrace& stacktrace);

class ConfigNode {
public:
    ConfigNode(YAML::Node a_node = YAML::Node(),
               NodeStacktrace a_node_names_path = {});

    // yaml-cpp functional

    YAML::NodeType::value Type() const;
    bool IsDefined() const;
    bool IsNull() const { return Type() == YAML::NodeType::Null; }
    bool IsScalar() const { return Type() == YAML::NodeType::Scalar; }
    bool IsSequence() const { return Type() == YAML::NodeType::Sequence; }
    bool IsMap() const { return Type() == YAML::NodeType::Map; }

    // bool conversions
    explicit operator bool() const { return IsDefined(); }
    bool operator!() const { return !IsDefined(); }

    // access
    template <typename T>
    T as() const {
        return m_node.as<T>();
    }
    template <typename T, typename S>
    T as(const S& fallback) const {
        return m_node.as<T>(fallback);
    }
    const std::string& Scalar() const;

    const std::string& Tag() const;
    void SetTag(const std::string& tag);

    // assignment
    bool is(const ConfigNode& rhs) const;

    // size/iterator
    std::size_t size() const;

    // const_iterator begin() const;
    // iterator begin();

    // const_iterator end() const;
    // iterator end();

    // indexing
    const ConfigNode operator[](std::string_view key) const;

    // node getters (use them is some functional is missing)

    const YAML::Node& get_node() const;
    YAML::Node& get_node();

private:
    YAML::Node m_node;
    NodeStacktrace m_node_names_path;
};

std::ostream& operator<<(std::ostream& out, const ConfigNode& node);

bool operator==(const ConfigNode& lhs, const ConfigNode& rhs);

}  // namespace sim

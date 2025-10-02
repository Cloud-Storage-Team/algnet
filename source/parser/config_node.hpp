#pragma once
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <string>
#include <vector>

namespace sim {
class ConfigNode {
public:
    using iterator = YAML::iterator;
    using const_iterator = YAML::const_iterator;

    ConfigNode(YAML::Node a_node = YAML::Node(),
               std::vector<std::string> a_node_names_path = {});

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
        return m_node.as<T>()
    }
    template <typename T, typename S>
    T as(const S& fallback) const {
        return m_node.as<T>(s);
    }
    const std::string& Scalar() const;

    const std::string& Tag() const;
    void SetTag(const std::string& tag);

    // assignment
    bool is(const ConfigNode& rhs) const;

    // size/iterator
    std::size_t size() const;

    const_iterator begin() const;
    iterator begin();

    const_iterator end() const;
    iterator end();

    // indexing
    const ConfigNode operator[](std::string_view key) const;

    // node getters (use them is some functional is missing)

    const YAML::Node& get_node() const;
    YAML::Node& get_node();

private:
    YAML::Node m_node;
    std::vector<std::string> m_node_names_path;
};

std::ostream& operator<<(std::ostream& out, const ConfigNode& node);

bool operator==(const ConfigNode& lhs, const ConfigNode& rhs);

}  // namespace sim

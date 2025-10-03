#pragma once
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace sim {

class ConfigNodeError : public std::runtime_error {
public:
    ConfigNodeError(std::string a_what);
};

class NodeStacktrace;

using NodeStacktracePtr = std::shared_ptr<NodeStacktrace>;
using NodePath = std::vector<std::string>;

// Simpli linked list like std::list, but on std::shared_ptr
class NodeStacktrace : public std::enable_shared_from_this<NodeStacktrace> {
public:
    friend class NodeStacktraceFactory;

    NodePath get_full_path() const;
    NodeStacktracePtr create_child(std::string key);

    std::optional<std::string> get_key() const;

    friend std::ostream& operator<<(std::ostream& out,
                                    NodeStacktracePtr stacktrace);

    std::string to_string() const;

private:
    // all contrcutros marked private to avoid creating NodeStacktrace not unded
    // std::shared_ptr

    // Creates root node
    NodeStacktrace();

    // Creates node py parent node and key by that this one got
    NodeStacktrace(NodeStacktracePtr a_parent_node, std::string a_key);

    // Invariant: only two states are present:
    // 1) m_parent_ptr = nullptr, m_key is std::nullopt
    // 2) m_parent_ptr is correct pointer, m_key contains NOT EMPTY value
    NodeStacktracePtr m_parent_ptr;
    std::optional<std::string> m_key;
};

class NodeStacktraceFactory {
public:
    static NodeStacktracePtr create_root();
    static NodeStacktracePtr create_from_parent(NodeStacktracePtr a_parent,
                                                std::string key);
};

// pointer to pair of previous node & key by which this node got
using NodePath = std::vector<std::string>;

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

    template <bool IsConst>
    class Iterator {
    public:
        using TypeName =
            std::conditional<IsConst, const ConfigNode, ConfigNode>::type;
        using YamlIterator =
            std::conditional<IsConst, YAML::Node::const_iterator,
                             YAML::Node::iterator>::type;

        using ThisIterator = Iterator<IsConst>;

        Iterator(YamlIterator a_it, NodeStacktracePtr a_stacktrace)
            : m_iterator(a_it), m_stacktrace_node(a_stacktrace) {}

        ThisIterator& operator++() {
            ++m_iterator;
            return *this;
        }

        ThisIterator operator++(int) {
            ThisIterator iterator_copy(*this);
            ++(*this);
            return iterator_copy;
        }

        bool operator==(const ThisIterator& rhs) const {
            return m_iterator == rhs.m_iterator;
        }

        bool operator!=(const ThisIterator& rhs) const {
            return m_iterator != rhs.m_iterator;
        }

        TypeName operator*() const {
            return TypeName(m_iterator->second,
                            m_stacktrace_node->create_child(
                                m_iterator->first.template as<std::string>()));
        }

    private:
        YamlIterator m_iterator;
        NodeStacktracePtr m_stacktrace_node;
    };

    using ConstIterator = Iterator<true>;
    using MutableIterator = Iterator<false>;

    ConstIterator begin() const;
    MutableIterator begin();

    ConstIterator end() const;
    MutableIterator end();

    // indexing
    const ConfigNode operator[](std::string_view key) const;

private:
    ConfigNode(YAML::Node a_node, NodeStacktracePtr a_node_ptr);

    // Invariant: m_node is correct, m_stacktrace_node is not null
    YAML::Node m_node;
    NodeStacktracePtr m_stacktrace_node;
};

std::ostream& operator<<(std::ostream& out, const ConfigNode& node);

bool operator==(const ConfigNode& lhs, const ConfigNode& rhs);

ConfigNode load_file(std::filesystem::path path);

}  // namespace sim

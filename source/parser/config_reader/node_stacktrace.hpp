#pragma once
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace sim {

class NodeStacktrace;

using NodeStacktracePtr = std::shared_ptr<const NodeStacktrace>;
using NodePath = std::vector<std::string>;

// Simple linked list like std::list, but on std::shared_ptr
class NodeStacktrace
    : public std::enable_shared_from_this<const NodeStacktrace> {
public:
    friend class NodeStacktraceFactory;

    NodePath get_full_path() const;
    NodeStacktracePtr create_child(std::string key) const;

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
    const NodeStacktracePtr m_parent_ptr;
    const std::optional<std::string> m_key;
};

class NodeStacktraceFactory {
public:
    static NodeStacktracePtr create_root();
    static NodeStacktracePtr create_from_parent(NodeStacktracePtr a_parent,
                                                std::string key);
};

}  // namespace sim

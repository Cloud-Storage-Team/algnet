#include "node_stacktrace.hpp"

#include <algorithm>
#include <sstream>

namespace sim {

NodeStacktrace::NodeStacktrace() : m_parent_ptr(nullptr), m_key(std::nullopt) {}

NodeStacktrace::NodeStacktrace(NodeStacktracePtr a_parent_ptr,
                               std::string a_key)
    : m_parent_ptr(a_parent_ptr), m_key(a_key) {
    if (m_parent_ptr == nullptr) {
        throw std::invalid_argument(
            "Link to parent node should not be null; if you want to create "
            "root node, use default constructor");
    }
    if (m_key.value().empty()) {
        throw std::invalid_argument(
            "Key is empty; if you want to create root node, use default "
            "constructor");
    }
}

NodePath NodeStacktrace::get_full_path() const {
    NodePath result;
    auto node = shared_from_this();
    while (node != nullptr) {
        result.emplace_back(node->m_key.value());
        node = node->m_parent_ptr;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

NodeStacktracePtr NodeStacktrace::create_child(std::string key) const {
    return NodeStacktraceFactory::create_from_parent(shared_from_this(),
                                                     std::move(key));
}

std::optional<std::string> NodeStacktrace::get_key() const { return m_key; }

std::ostream& operator<<(std::ostream& out, NodeStacktracePtr node) {
    NodePath path = node->get_full_path();
    std::string prefix_space = "";
    std::size_t size = path.size();
    for (std::size_t i = 0; i < size; i++) {
        out << prefix_space << path[i];
        prefix_space += "  ";  // Add two spaces to modulate yaml file structure
        if (i + 1 < size) {
            out << ":\n";
        }
    }
    return out;
}

NodeStacktracePtr NodeStacktraceFactory::create_root() {
    // used raw constructor because std::make_shared can not be used
    // (std::shared_ptr is not a friend of NodeStacktrace but uses private
    // constructor)
    return NodeStacktracePtr(new NodeStacktrace());
}

NodeStacktracePtr NodeStacktraceFactory::create_from_parent(
    NodeStacktracePtr parent, std::string key) {
    // used raw constructor because std::make_shared can not be used
    // (std::shared_ptr is not a friend of NodeStacktrace but uses private
    // constructor)
    return NodeStacktracePtr(new NodeStacktrace(parent, std::move(key)));
}

std::string NodeStacktrace::to_string() const {
    std::stringstream ss;
    ss << shared_from_this();
    return ss.str();
}

}  // namespace sim

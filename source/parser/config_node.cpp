#include "config_node.hpp"

namespace sim {

ConfigNodeError::ConfigNodeError(std::string a_what)
    : std::runtime_error(std::move(a_what)) {}

std::ostream& operator<<(std::ostream& out, const NodeStacktrace& stacktrace) {
    std::string prefix_space = "";
    size_t size = stacktrace.size();
    for (size_t i = 0; i < size; i++) {
        out << prefix_space << stacktrace[i];
        prefix_space += "  ";  // add two more spaces to model YAML structure
        if (i + 1 < size) {
            out << ":\n";
        }
    }
    return out;
}

std::string to_string(const NodeStacktrace& stacktrace) {
    std::stringstream stream;
    stream << stacktrace;
    return stream.str();
}

ConfigNode::ConfigNode(YAML::Node a_node,
                       std::vector<std::string> a_node_names_path)
    : m_node(std::move(a_node)),
      m_node_names_path(std::move(a_node_names_path)) {
    if (!m_node) {
        throw ConfigNodeError(
            "Can not construct ConfigNode: given YAML::Node is invalid; "
            "stacktrace:\n" +
            to_string(m_node_names_path));
    }
}

YAML::NodeType::value ConfigNode::Type() const { return m_node.Type(); }

}  // namespace sim
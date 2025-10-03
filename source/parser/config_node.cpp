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
    : m_node(std::move(a_node)), m_stacktrace(std::move(a_node_names_path)) {
    if (!m_node) {
        throw ConfigNodeError(
            "Can not construct ConfigNode: given YAML::Node is invalid; "
            "stacktrace:\n" +
            to_string(m_stacktrace));
    }
}

YAML::NodeType::value ConfigNode::Type() const { return m_node.Type(); }

bool ConfigNode::IsDefined() const { return m_node.IsDefined(); }
bool ConfigNode::IsNull() const { return m_node.IsNull(); }
bool ConfigNode::IsScalar() const { return m_node.IsScalar(); }
bool ConfigNode::IsSequence() const { return m_node.IsSequence(); }
bool ConfigNode::IsMap() const { return m_node.IsMap(); }

// bool conversions
ConfigNode::operator bool() const { return m_node.operator bool(); }
bool ConfigNode::operator!() const { return m_node.operator!(); }

const std::string& ConfigNode::Scalar() const { return m_node.Scalar(); }

const std::string& ConfigNode::Tag() const { return m_node.Tag(); }

// size/iterator
std::size_t ConfigNode::size() const { return m_node.size(); }

const ConfigNode ConfigNode::operator[](std::string_view key) const {
    const YAML::Node child_node = m_node[key];
    if (!child_node) {
        std::stringstream ss;
        ss << "Key error: node\n";
        ss << m_stacktrace << '\n';
        ss << "have no key `" << key << "\'";
        throw ConfigNodeError(ss.str());
    }
    NodeStacktrace child_stacktrace(m_stacktrace);
    child_stacktrace.emplace_back(key);
    return ConfigNode(child_node, child_stacktrace);
};

}  // namespace sim
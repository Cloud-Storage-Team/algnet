#include "config_node.hpp"

namespace sim {

ConfigNodeError::ConfigNodeError(std::string a_what)
    : std::runtime_error(std::move(a_what)) {}

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

NodeStacktracePtr NodeStacktrace::create_child(std::string key) {
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
    // used raw contsructor because std::make_shared can not be used
    // (std::shared_ptr is not a friend of NodeStacktrace but uses private
    // constructor)
    return NodeStacktracePtr(new NodeStacktrace());
}

NodeStacktracePtr NodeStacktraceFactory::create_from_parent(
    NodeStacktracePtr parent, std::string key) {
    // used raw contsructor because std::make_shared can not be used
    // (std::shared_ptr is not a friend of NodeStacktrace but uses private
    // constructor)
    return NodeStacktracePtr(new NodeStacktrace(parent, std::move(key)));
}

std::string NodeStacktrace::to_string() const {
    std::stringstream ss;
    ss << shared_from_this();
    return ss.str();
}

ConfigNode::ConfigNode(YAML::Node a_node)
    : ConfigNode(std::move(a_node), NodeStacktraceFactory::create_root()) {};

ConfigNode::ConfigNode(YAML::Node a_node, NodeStacktracePtr a_ptr)
    : m_node(std::move(a_node)), m_stacktrace_node(a_ptr) {
    if (m_stacktrace_node == nullptr) {
        throw ConfigNodeError(
            "Can not construct ConfigNode: stacktrace node pointer is null");
    }
    if (!m_node) {
        throw ConfigNodeError(
            "Can not construct ConfigNode: given YAML::Node is "
            "invalid; stacktrace:\n" +
            m_stacktrace_node->to_string());
    }
}

std::optional<std::string> ConfigNode::get_name() const {
    return m_stacktrace_node->get_key();
}

const YAML::Node& ConfigNode::get_node() const { return m_node; }

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

ConfigNode::ConstIterator ConfigNode::begin() const {
    return ConstIterator(m_node.begin(), m_stacktrace_node);
}

ConfigNode::ConstIterator ConfigNode::end() const {
    return ConstIterator(m_node.end(), m_stacktrace_node);
}

ConfigNode::MutableIterator ConfigNode::begin() {
    return MutableIterator(m_node.begin(), m_stacktrace_node);
}

ConfigNode::MutableIterator ConfigNode::end() {
    return MutableIterator(m_node.end(), m_stacktrace_node);
}

const ConfigNode ConfigNode::operator[](std::string_view key) const {
    const YAML::Node child_node = m_node[key];
    if (!child_node) {
        std::stringstream ss;
        ss << "Key error: node\n";
        ss << m_stacktrace_node << '\n';
        ss << "have no key `" << key << "\'";
        throw ConfigNodeError(ss.str());
    }
    NodeStacktracePtr child_stacktrace =
        m_stacktrace_node->create_child(std::string(key));
    return ConfigNode(std::move(child_node), child_stacktrace);
};

ConfigNode load_file(std::filesystem::path path) {
    return ConfigNode(YAML::LoadFile(path));
}

}  // namespace sim
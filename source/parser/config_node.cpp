#include "config_node.hpp"

namespace sim {

ConfigNodeError::ConfigNodeError(std::string a_what)
    : std::runtime_error(std::move(a_what)) {}

NodeStacktrace::NodeStacktrace() : m_parent_ptr(nullptr), m_key() {}

NodeStacktrace::NodeStacktrace(NodeStacktracePtr a_link_to_prev,
                               std::string a_key)
    : m_parent_ptr(a_link_to_prev), m_key(a_key) {
    if (m_parent_ptr == nullptr) {
        throw std::invalid_argument(
            "Link to parent node should not be null; if you want to create "
            "root node, use default constructor");
    }
}

NodeStacktracePtr NodeStacktrace::create_child(std::string key) {
    return NodeStacktraceFactory::create_from_parent(shared_from_this(),
                                                     std::move(key));
}

NodePath NodeStacktrace::get_full_path() const {
    NodePath result;
    auto node = shared_from_this();
    while (node != nullptr) {
        result.emplace_back(node->m_key);
        node = node->m_parent_ptr;
    }
    std::reverse(result.begin(), result.end());
    return result;
}
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
    : ConfigNode(std::move(a_node), nullptr) {};

ConfigNode::ConfigNode(YAML::Node a_node, NodeStacktracePtr a_ptr)
    : m_node(std::move(a_node)), m_stacktrace(a_ptr) {
    if (!m_node) {
        std::string errorMessage =
            "Can not construct ConfigNode: given YAML::Node is "
            "invalid";
        if (m_stacktrace != nullptr) {
            errorMessage += "; stacktrace:\n" + m_stacktrace->to_string();
        }
        throw ConfigNodeError(std::move(errorMessage));
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

ConfigNode::ConstIterator ConfigNode::begin() const {
    return ConstIterator(m_node.begin(), m_stacktrace);
}

ConfigNode::ConstIterator ConfigNode::end() const {
    return ConstIterator(m_node.end(), m_stacktrace);
}

ConfigNode::MutableIterator ConfigNode::begin() {
    return MutableIterator(m_node.begin(), m_stacktrace);
}

ConfigNode::MutableIterator ConfigNode::end() {
    return MutableIterator(m_node.end(), m_stacktrace);
}

const ConfigNode ConfigNode::operator[](std::string_view key) const {
    const YAML::Node child_node = m_node[key];
    if (!child_node) {
        std::stringstream ss;
        ss << "Key error: node\n";
        ss << m_stacktrace << '\n';
        ss << "have no key `" << key << "\'";
        throw ConfigNodeError(ss.str());
    }
    NodeStacktracePtr child_stacktrace =
        m_stacktrace->create_child(std::string(key));
    return ConfigNode(std::move(child_node), child_stacktrace);
};

const YAML::Node& ConfigNode::get_node() const { return m_node; }

YAML::Node ConfigNode::get_node() { return m_node; }

ConfigNode load_file(std::filesystem::path path) {
    return ConfigNode(YAML::LoadFile(path));
}

}  // namespace sim
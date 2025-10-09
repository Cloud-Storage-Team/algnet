#include "config_node.hpp"

namespace sim {

ConfigNodeError::ConfigNodeError(std::string a_what)
    : std::runtime_error(std::move(a_what)) {}

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

NodeStacktracePtr ConfigNode::get_stacktrace() const {
    return m_stacktrace_node;
}

ConfigNodeError ConfigNode::create_parsing_error(std::string_view error) const {
    std::stringstream ss;
    ss << "Error while parsing node\n";
    ss << m_stacktrace_node << '\n';
    ss << error << '\n';
    return ConfigNodeError(ss.str());
}

YAML::NodeType::value ConfigNode::Type() const { return m_node.Type(); }

bool ConfigNode::IsNull() const { return m_node.IsNull(); }
bool ConfigNode::IsScalar() const { return m_node.IsScalar(); }
bool ConfigNode::IsSequence() const { return m_node.IsSequence(); }
bool ConfigNode::IsMap() const { return m_node.IsMap(); }

const std::string& ConfigNode::Scalar() const { return m_node.Scalar(); }

const std::string& ConfigNode::Tag() const { return m_node.Tag(); }

// size/iterator
std::size_t ConfigNode::size() const { return m_node.size(); }

ConfigNode::Iterator::Iterator(YAML::const_iterator a_it,
                               NodeStacktracePtr a_stacktrace)
    : m_iterator(a_it), m_stacktrace_node(a_stacktrace) {
    if (m_stacktrace_node == nullptr) {
        throw ConfigNodeError(
            "Can not construct config node iterator: stacktrace pointer is "
            "null");
    }
}

ConfigNode::Iterator& ConfigNode::Iterator::operator++() {
    ++m_iterator;
    return *this;
}

ConfigNode::Iterator ConfigNode::Iterator::operator++(int) {
    Iterator iterator_copy(*this);
    ++(*this);
    return iterator_copy;
}

bool ConfigNode::Iterator::operator==(const Iterator& rhs) const {
    return m_iterator == rhs.m_iterator;
}

bool ConfigNode::Iterator::operator!=(const Iterator& rhs) const {
    return m_iterator != rhs.m_iterator;
}

ConfigNode ConfigNode::Iterator::operator*() const {
    YAML::Node key_node = m_iterator->first;
    YAML::Node value_node = m_iterator->second;
    try {
        std::string key = key_node.as<std::string>();
        return ConfigNode(m_iterator->second,
                          m_stacktrace_node->create_child(std::move(key)));
    } catch (const YAML::Exception& e) {
        throw ConfigNodeError(
            "Can not take value under config node iterator; node "
            "stacktrace:\n" +
            m_stacktrace_node->to_string() + "\nError message: " + e.what());
    }
}

ConfigNode::Iterator ConfigNode::begin() const {
    return Iterator(m_node.begin(), m_stacktrace_node);
}

ConfigNode::Iterator ConfigNode::end() const {
    return Iterator(m_node.end(), m_stacktrace_node);
}

utils::StrExpected<ConfigNode> ConfigNode::operator[](
    std::string_view key) const {
    const YAML::Node child_node = m_node[key];
    if (!child_node) {
        std::stringstream ss;
        ss << "Key error: node\n";
        ss << m_stacktrace_node << '\n';
        ss << "does not have key `" << key << '`';
        return std::unexpected(ss.str());
    }
    NodeStacktracePtr child_stacktrace =
        m_stacktrace_node->create_child(std::string(key));
    return ConfigNode(std::move(child_node), child_stacktrace);
};

ConfigNode load_file(std::filesystem::path path) {
    return ConfigNode(YAML::LoadFile(path.string()));
}

}  // namespace sim

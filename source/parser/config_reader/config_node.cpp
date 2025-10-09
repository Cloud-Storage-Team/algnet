#include "config_node.hpp"

namespace sim {

ConfigNodeError::ConfigNodeError(std::string a_what)
    : std::runtime_error(std::move(a_what)) {}
ConfigNode::ConfigNode(YAML::Node a_node, std::optional<std::string> a_name)
    : m_node(std::move(a_node)), m_name(std::move(a_name)) {
    if (!m_node) {
        throw ConfigNodeError(
            "Can not construct ConfigNode: given YAML::Node is "
            "invalid");
    }
}

const YAML::Node& ConfigNode::get_node() const { return m_node; }

const std::optional<std::string>& ConfigNode::get_name() const {
    return m_name;
}

const std::string& ConfigNode::get_name_or_throw() const {
    if (!m_name) {
        throw create_parsing_error("Node does not have name");
    }
    return m_name.value();
}

ConfigNodeError ConfigNode::create_parsing_error(std::string_view error) const {
    std::stringstream ss;
    ss << "Error while parsing node " << *this << ":\n";
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

ConfigNode::Iterator::Iterator(YAML::const_iterator a_it) : m_iterator(a_it) {}

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
    YAML::detail::iterator_value iterator_value = *m_iterator;
    // Call method of Node to get which type of nodes iterator goes
    // Explanation: iterator_value inherited from
    // YAML::Node and std::pair<YAML::Node, YAML::Node>, but only one value of
    // this pair is correct (please, them about std::variant...)
    if (iterator_value) {
        // iterator goes over "named" nodes like
        // list:
        //  - value_0
        //  - value_1
        //  ...
        YAML::Node node = iterator_value;
        return ConfigNode(node, std::nullopt);
    } else {
        // iterator goes over "named" nodes like
        // list:
        //  node_1: ...
        //  node_2: ...
        //  ...
        YAML::Node key_node = m_iterator->first;
        YAML::Node value_node = m_iterator->second;
        if (!key_node || !value_node) {
            throw ConfigNodeError(
                "Can not take value under config node iterator; all possile "
                "nodes are invalid");
        }
        std::string key = key_node.as<std::string>();
        return ConfigNode(value_node, key);
    }
}

ConfigNode::Iterator ConfigNode::begin() const {
    return Iterator(m_node.begin());
}

ConfigNode::Iterator ConfigNode::end() const { return Iterator(m_node.end()); }

utils::StrExpected<ConfigNode> ConfigNode::operator[](
    std::string_view key) const {
    const YAML::Node child_node = m_node[key];
    if (!child_node) {
        std::stringstream ss;
        ss << "Key error: node " << *this << ":\n";
        ss << "does not have key `" << key << '`';
        return std::unexpected(ss.str());
    }
    return ConfigNode(std::move(child_node), std::string(key));
};

std::ostream& operator<<(std::ostream& out, const ConfigNode& node) {
    YAML::Mark mark = node.m_node.Mark();
    if (node.m_name) {
        out << "name: '" << node.m_name.value() << "'";
    } else {
        out << "without name";
    }
    return out << " at line " << mark.line + 1 << " column " << mark.column + 1;
}

ConfigNode load_file(std::filesystem::path path) {
    return ConfigNode(YAML::LoadFile(path.string()));
}

}  // namespace sim

#include "config_node.hpp"

namespace sim {

ConfigNode::ConfigNode(YAML::Node a_node, std::optional<std::string> a_name)
    : m_node(std::move(a_node)), m_name(std::move(a_name)) {
    if (!m_node) {
        throw std::runtime_error(
            "Can not construct ConfigNode: given YAML::Node is "
            "invalid");
    }
}

const YAML::Node& ConfigNode::get_node() const noexcept { return m_node; }

const std::optional<std::string>& ConfigNode::get_name() const noexcept {
    return m_name;
}

const std::string& ConfigNode::get_name_or_throw() const {
    if (!m_name) {
        throw create_parsing_error("Node does not have name");
    }
    return m_name.value();
}

std::runtime_error ConfigNode::create_parsing_error(
    std::string_view error) const {
    std::stringstream ss;
    ss << "Error while parsing node " << *this << ":\n";
    ss << error << '\n';
    return std::runtime_error(ss.str());
}

std::ostream& operator<<(std::ostream& out, const ConfigNode& node) {
    YAML::Mark mark = node.m_node.Mark();
    if (node.m_name) {
        out << "name: '" << node.m_name.value() << "'";
    } else {
        out << "without name";
    }
    if (mark.line >= 0 && mark.column >= 0) {
        out << " at line " << mark.line + 1 << " column " << mark.column + 1;
    } else {
        out << " at unknown location";
    }
    return out;
}

YAML::NodeType::value ConfigNode::Type() const { return m_node.Type(); }

bool ConfigNode::IsNull() const noexcept { return m_node.IsNull(); }
bool ConfigNode::IsScalar() const noexcept { return m_node.IsScalar(); }
bool ConfigNode::IsSequence() const noexcept { return m_node.IsSequence(); }
bool ConfigNode::IsMap() const noexcept { return m_node.IsMap(); }

const std::string& ConfigNode::Tag() const noexcept { return m_node.Tag(); }

// size/iterator
std::size_t ConfigNode::size() const noexcept { return m_node.size(); }

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
    // Explanation: iterator_value under *m_iterator inherited from
    // YAML::Node and std::pair<YAML::Node, YAML::Node>, but only one value of
    // this pair is correct (please, tell them about std::variant...)
    if (m_iterator->IsDefined()) {
        // iterator goes over "named" nodes like
        // list:
        //  - value_0
        //  - value_1
        //  ...
        YAML::Node node = *m_iterator;
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
            throw std::runtime_error(
                "Can not take value under config node iterator; all possible "
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
    if (child_node.IsNull()) {
        // if node is null, its name should be empty
        return ConfigNode(std::move(child_node), std::nullopt);
    }
    return ConfigNode(std::move(child_node), std::string(key));
};

ConfigNode load_file(std::filesystem::path path) {
    return ConfigNode(YAML::LoadFile(path.string()));
}

}  // namespace sim

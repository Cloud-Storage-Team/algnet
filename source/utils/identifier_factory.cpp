#include "utils/identifier_factory.hpp"

namespace sim {

bool IdentifierFactory::add_object(std::shared_ptr<Identifiable> object) {
    Id id = object->get_id();
    if (m_id_table.find(id) != m_id_table.end() || id == "") {
        return false;
    }
    m_id_table[id] = object;
    return true;
}

bool IdentifierFactory::delete_object(std::shared_ptr<Identifiable> object) {
    Id id = object->get_id();
    if (auto it = m_id_table.find(id); it != m_id_table.end()) {
        m_id_table.erase(it);
        return true;
    }
    return false;
}

void IdentifierFactory::clear() {
    std::map<Id, std::shared_ptr<Identifiable> >().swap(m_id_table);
}

}  // namespace sim

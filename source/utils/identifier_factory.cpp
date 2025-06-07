#include "utils/identifier_factory.hpp"

namespace sim {

void IdentifierFactory::clear() {
    std::map<Id, std::shared_ptr<Identifiable> >().swap(m_id_table);
}

}  // namespace sim

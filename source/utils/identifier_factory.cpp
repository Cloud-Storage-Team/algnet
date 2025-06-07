#include "utils/identifier_factory.hpp"
#include <map>
#include <memory>

namespace sim {

void IdentifierFactory::clear() {
    std::map<Id, std::shared_ptr<Identifiable> >().swap(m_id_table);
}

}  // namespace sim

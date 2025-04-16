#include "identifier_factory.hpp"

namespace sim {

Id IdentifierFactory::generate_id() { return next_id++; }

}  // namespace sim

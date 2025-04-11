#include <iterator>
#include <set>
#include <memory>

#include "loop_iterator.hpp"
#include "link.hpp"


namespace sim {

    template class LoopIterator<std::set<std::shared_ptr<ILink>>::iterator>;

}  // namespace sim

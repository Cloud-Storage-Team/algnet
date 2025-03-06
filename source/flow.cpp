#include "flow.hpp"

namespace sim {

Flow::Flow(ISender *a_src, IReceiver *a_dest, float a_start_cwnd)
    : m_src(a_src),
      m_dest(a_dest),
      m_nacks(0),
      m_cwnd(a_start_cwnd),
      m_sent_bytes(0) {}

IReceiver *Flow::get_destination() const { return m_dest; }

}  // namespace sim
#pragma once

#include "node.hpp"

namespace sim {

class Flow {
  public:
    // Start at time
    void start(int time);

    // Generate a new packet
    bool generate(int packet_size);

    // Update the internal state according to some congestion control algorithm
    void update();

  private:
    Node *m_src;
    int m_nacks;
    int m_cwnd;
    int m_sent_bytes;
};

}  // namespace sim

#pragma once
#include "i_receiver.hpp"
#include "i_sender.hpp"

namespace sim {

class IHost : public ISender, public IReceiver {
public:
    virtual ~IHost() = default;
};

}  // namespace sim

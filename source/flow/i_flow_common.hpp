#pragma once
#include "device/interfaces/i_host.hpp"
#include "utils/identifier_factory.hpp"

namespace sim {

class IFlowCommon : public Identifiable {
public:
    virtual std::shared_ptr<IHost> get_sender() const = 0;
    virtual std::shared_ptr<IHost> get_receiver() const = 0;
};

}  // namespace sim
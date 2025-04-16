#pragma once

#include "types.hpp"

namespace sim {

class IdentifierFactory {
public:
    static IdentifierFactory& get_instance() {
        static IdentifierFactory instance;
        return instance;
    }

    Id generate_id();

private:
    IdentifierFactory() {}
    IdentifierFactory(const IdentifierFactory&) = delete;
    IdentifierFactory& operator=(const IdentifierFactory&) = delete;

    Id next_id = 0;
};

class Identifiable {
public:
    virtual Id get_id() const = 0;
};

}  // namespace sim

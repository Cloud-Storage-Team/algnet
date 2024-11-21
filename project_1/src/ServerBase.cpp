#include "ServerBase.hpp"

ServerBase::ServerBase(std::uint64_t id)
    : id(id) { }

std::uint64_t ServerBase::GetID() const {
    return id;
}
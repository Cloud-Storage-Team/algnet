#pragma once
#include <stdexcept>
#include <string>

namespace utils {

class BaseError : public std::runtime_error {
public:
    explicit BaseError(std::string message);
    std::string what_str() const;
};

}  // namespace utils
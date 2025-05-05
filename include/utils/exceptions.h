#pragma once

#include <stdexcept>

namespace utils {

class BadFormat : public std::invalid_argument {
 public:
  using invalid_argument::invalid_argument;
};

}  // namespace utils

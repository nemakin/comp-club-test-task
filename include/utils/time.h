#pragma once

#include <cassert>
#include <chrono>
#include <format>
#include <stdexcept>

namespace utils::time {

using namespace std::chrono_literals;
using Time = std::chrono::minutes;

inline Time FromString(const std::string& hh_mm) {
  Time t;
  if (std::istringstream ss(hh_mm); !(ss >> std::chrono::parse("%H:%M", t))) {
    throw std::invalid_argument("time::FromString: " + hh_mm);
  }
  return t;
}

inline std::string ToString(Time t) { return std::format("{:%H:%M}", t % 24h); }

class Interval {
 private:
  Time begin_, end_;

 public:
  Interval() = default;
  Interval(Time begin, Time end) : begin_(begin), end_(end) {
    if (end_ < begin_) {
      end_ += 24h;
    } else if (begin_ + 24h < end_) {
      end_ -= 24h;
    }
  }
  Interval(const std::string& hh_mm_from, const std::string& hh_mm_to)
      : Interval(FromString(hh_mm_from), FromString(hh_mm_to)) {}
  Time Begin() const { return begin_; }
  Time End() const { return end_ % 24h; }
  bool Contains(Time t) const {
    if (t < begin_) {
      t += 24h;
    }
    return t >= begin_ && t <= end_;
  }
  bool operator==(const Interval& other) const = default;
};

}  // namespace utils::time

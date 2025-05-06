#pragma once

#include <cassert>
#include <chrono>
#include <string>

#include "utils/time.h"

namespace club {

class Table {
 public:
  enum class Status { Occupied, Free };
  Status Status() const { return status_; }
  std::string ToString() const {
    return std::to_string(total_profit_) + ' ' + utils::time::ToString(total_time_occupied_);
  }
  void Occupy(utils::time::Time at) {
    assert(status_ == Status::Free);
    SetStatus(Status::Occupied, at);
  }
  void Free(utils::time::Time t, int cost_per_hour) {
    assert(status_ == Status::Occupied);
    using namespace std::chrono_literals;
    utils::time::Time mins_occupied = (t - last_time_occupied_);
    int hours_occupied = mins_occupied / 1h;
    hours_occupied += (mins_occupied % 1h).count() == 0 ? 0 : 1;
    total_profit_ += cost_per_hour * hours_occupied;
    SetStatus(Status::Free, t);
  }

 private:
  void SetStatus(enum Status s, utils::time::Time t) {
    using namespace std::chrono_literals;
    status_ = s;
    if (status_ == Table::Status::Free) {
      total_time_occupied_ += t - last_time_occupied_;
    } else {
      last_time_occupied_ = t >= last_time_occupied_ ? t : t + 24h;
    }
  }

 private:
  enum Status status_ = Status::Free;
  int total_profit_ = 0;
  utils::time::Time last_time_occupied_;
  utils::time::Time total_time_occupied_{0};
};

};  // namespace club

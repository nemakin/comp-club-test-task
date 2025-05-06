#pragma once

#include <cassert>
#include <list>
#include <optional>
#include <string_view>

namespace club {

class ClientInfo {
 public:
  using QueueTicket = std::list<std::string_view>::iterator;

  enum class Status { Pending, Playing, Waiting };
  Status Status() const { return status_; }

  int TableNumber() const {
    assert(status_ == Status::Playing && table_number_.has_value());
    return table_number_.value();
  }
  void GiveTicket(QueueTicket ticket) {
    queue_pos_ = ticket;
    status_ = Status::Waiting;
  }
  void Sit(int table_number) {
    table_number_ = table_number;
    status_ = Status::Playing;
  }
  QueueTicket Ticket() const {
    assert(status_ == Status::Waiting && queue_pos_.has_value());
    return queue_pos_.value();
  }

 private:
  enum Status status_ = Status::Pending;
  std::optional<int> table_number_;
  std::optional<QueueTicket> queue_pos_;
};

};  // namespace club

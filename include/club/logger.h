#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <ostream>

#include "club/event.h"
#include "utils/time.h"

namespace club {

class EventLogger {
 public:
  explicit EventLogger(std::ostream& os) : os_(os) {}
  void LogMessage(const std::string& msg) { os_ << msg << std::endl; }
  void LogEvent(const events::Event& e) {
    LogMessage(e.ToString());
    last_event_time_ = e.Time();
  }
  auto LastEventTime() const { return last_event_time_; }

 private:
  std::optional<utils::time::Time> last_event_time_;
  std::ostream& os_;
};

}  // namespace club

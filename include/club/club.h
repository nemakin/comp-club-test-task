#pragma once

#include <iostream>
#include <list>
#include <string>
#include <string_view>
#include <vector>

#include "club/client.h"
#include "club/config.h"
#include "club/event.h"
#include "club/logger.h"
#include "club/table.h"

namespace club {

class ComputerClub {
  using EventPtr = events::EventPtr;

 public:
  ComputerClub(Config cfg, EventLogger logger = EventLogger(std::cout));
  ~ComputerClub();

  void ProcessIncomingEvent(EventPtr p);

 private:
  void ValidateEvent(const events::Event& e);

  void ProcessClientArrived(const events::ClientEvent& e);
  void ProcessClientSatDown(const events::TableEvent& e);
  void ProcessClientWaiting(const events::ClientEvent& e);
  void ProcessClientLeft(const events::ClientEvent& e);

  void ProcessGeneratedEvent(EventPtr p);

 private:
  struct StringHash {
    using is_transparent = void;
    std::size_t operator()(const char* s) const {
      return std::hash<std::string_view>{}(s);
    }
    std::size_t operator()(std::string_view sv) const {
      return std::hash<std::string_view>{}(sv);
    }
    std::size_t operator()(const std::string& s) const {
      return std::hash<std::string>{}(s);
    }
  };

  void FreeTable(size_t number, utils::time::Time at);
  void OccupyTable(size_t number, utils::time::Time at);

  Config cfg_;
  EventLogger logger_;

  size_t free_tables_;
  std::list<std::string_view> wait_queue_;
  std::vector<Table> tables_;
  std::unordered_map<std::string, ClientInfo, StringHash, std::equal_to<>>
      clients_;
};

}  // namespace club

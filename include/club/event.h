#pragma once

#include <memory>

#include "utils/time.h"

namespace club::events {

enum class IdIncoming {
  kClientArrived = 1,
  kClientSatDown = 2,
  kClientWaiting = 3,
  kClientLeft = 4,
};

enum class IdGenerated {
  kClientLeft = 11,
  kClientSat = 12,
  kError = 13,
};

namespace errors {

constexpr auto kClientAlreadyThere = "YouShallNotPass";
constexpr auto kClubIsClosed = "NotOpenYet";
constexpr auto kPlaceIsOccupied = "PlaceIsBusy";
constexpr auto kUknownClient = "ClientUnknown";
constexpr auto kFreePlaceAvailable = "ICanWaitNoLonger!";

}  // namespace errors

class Event {
 public:
  Event(utils::time::Time t, int id) : time_(t), id_(id) {}
  int Id() const { return id_; }
  utils::time::Time Time() const { return time_; }

  virtual std::string ToString() const { return utils::time::ToString(time_) + ' ' + std::to_string(id_); }
  virtual ~Event() = default;

 protected:
  utils::time::Time time_;
  int id_;
};

using EventPtr = std::unique_ptr<events::Event>;

class ErrorEvent : public Event {
 public:
  ErrorEvent(utils::time::Time t, std::string_view error_msg)
      : Event(t, static_cast<int>(IdGenerated::kError)), error_(error_msg) {}
  virtual std::string ToString() const override { return Event::ToString() + ' ' + std::string(error_); }
  static EventPtr Make(utils::time::Time t, std::string error_msg) {
    return std::make_unique<ErrorEvent>(t, error_msg);
  }

 private:
  std::string_view error_;
};

class ClientEvent : public Event {
 public:
  ClientEvent(utils::time::Time t, int id, std::string name) : Event(t, id), client_name_(name) {}
  const std::string& Name() const { return client_name_; }

  std::string ToString() const override { return Event::ToString() + ' ' + client_name_; }
  static EventPtr Make(utils::time::Time t, int id, std::string name) {
    return std::make_unique<ClientEvent>(t, id, name);
  }

 protected:
  std::string client_name_;
};

class TableEvent : public ClientEvent {
 public:
  TableEvent(utils::time::Time t, int id, std::string name, int table)
      : ClientEvent(t, id, name), table_number_(table) {}
  size_t TableNumber() const { return table_number_; }

  std::string ToString() const override { return ClientEvent::ToString() + ' ' + std::to_string(table_number_); }

  static EventPtr Make(utils::time::Time t, int id, std::string name, int table) {
    return std::make_unique<TableEvent>(t, id, name, table);
  }

 private:
  size_t table_number_;
};

EventPtr FromString(const std::string& line);

}  // namespace club::events

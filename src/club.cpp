#include "club/club.h"

#include <algorithm>

using namespace club::events;

namespace club {

ComputerClub::ComputerClub(Config cfg, EventLogger logger)
    : cfg_(cfg), logger_(logger), free_tables_(cfg_.table_count), tables_(free_tables_ + 1) {
  logger_.LogMessage(utils::time::ToString(cfg_.working_hours.Begin()));
}

void ComputerClub::ProcessIncomingEvent(EventPtr p) {
  ValidateEvent(*p);
  switch (static_cast<IdIncoming>(p->Id())) {
    using enum IdIncoming;
    case kClientArrived:
      logger_.LogEvent(*p);
      ProcessClientArrived(static_cast<const ClientEvent&>(*p));
      break;
    case kClientSatDown:
      logger_.LogEvent(*p);
      ProcessClientSatDown(static_cast<const TableEvent&>(*p));
      break;
    case kClientWaiting:
      logger_.LogEvent(*p);
      ProcessClientWaiting(static_cast<const ClientEvent&>(*p));
      break;
    case kClientLeft:
      logger_.LogEvent(*p);
      ProcessClientLeft(static_cast<const ClientEvent&>(*p));
      break;
    default:
      throw std::invalid_argument("Uknown event ID: " +
                                  std::to_string(p->Id()));
  }
}
ComputerClub::~ComputerClub() {
  std::vector<std::string_view> remaining_clients;
  remaining_clients.reserve(clients_.size());
  std::ranges::for_each(
      clients_, [&](const auto& p) { remaining_clients.push_back(p.first); });
  std::ranges::sort(remaining_clients);

  std::ranges::for_each(remaining_clients, [this](auto s) {
    const auto& [name, info] = *clients_.find(s);
    if (info.Status() == ClientInfo::Status::Playing) {
      FreeTable(info.TableNumber(), cfg_.working_hours.End());
    }
    ProcessGeneratedEvent(
        ClientEvent::Make(cfg_.working_hours.End(),
                          static_cast<int>(IdGenerated::kClientLeft), name));
  });

  logger_.LogMessage(utils::time::ToString(cfg_.working_hours.End()));
  for (size_t i = 1; i <= cfg_.table_count; ++i) {
    logger_.LogMessage(std::to_string(i) + ' ' + tables_[i].ToString());
  }
}

void ComputerClub::OccupyTable(size_t number, utils::time::Time at) {
  tables_[number].Occupy(at);
  --free_tables_;
}
void ComputerClub::FreeTable(size_t number, utils::time::Time at) {
  tables_[number].Free(at, cfg_.cost_per_hour);
  ++free_tables_;
}

void ComputerClub::ProcessGeneratedEvent(EventPtr p) {
  switch (static_cast<IdGenerated>(p->Id())) {
    using enum IdGenerated;
    case kClientLeft:
    case kClientSat:
    case kError:
      logger_.LogEvent(*p);
      return;
    default:
      throw std::invalid_argument("Uknown event ID: " +
                                  std::to_string(p->Id()));
  }
}
void ComputerClub::ProcessClientArrived(const ClientEvent& e) {
  if (clients_.find(e.Name()) != clients_.cend()) {
    ProcessGeneratedEvent(
        ErrorEvent::Make(e.Time(), errors::kClientAlreadyThere));
    return;
  }
  if (!cfg_.working_hours.Contains(e.Time())) {
    ProcessGeneratedEvent(ErrorEvent::Make(e.Time(), errors::kClubIsClosed));
    return;
  }
  clients_[e.Name()] = ClientInfo{};
}

void ComputerClub::ProcessClientSatDown(const TableEvent& e) {
  if (clients_.find(e.Name()) == clients_.cend()) {
    ProcessGeneratedEvent(ErrorEvent::Make(e.Time(), errors::kUknownClient));
    return;
  }

  int new_table_number = e.TableNumber();
  if (tables_[new_table_number].Status() == Table::Status::Occupied) {
    ProcessGeneratedEvent(ErrorEvent::Make(e.Time(), errors::kPlaceIsOccupied));
    return;
  }

  auto& client = clients_[e.Name()];
  if (client.Status() == ClientInfo::Status::Playing) {
    FreeTable(client.TableNumber(), e.Time());
  }
  OccupyTable(new_table_number, e.Time());

  client.Sit(new_table_number);
}

void ComputerClub::ProcessClientWaiting(const ClientEvent& e) {
  auto it = clients_.find(e.Name());
  if (it == clients_.cend()) {
    ProcessGeneratedEvent(ErrorEvent::Make(e.Time(), errors::kUknownClient));
    return;
  }

  if (clients_[e.Name()].Status() == ClientInfo::Status::Waiting) {
    ProcessGeneratedEvent(
        ErrorEvent::Make(e.Time(), errors::kClientAlreadyThere));
    return;
  }

  if (free_tables_ != 0) {
    ProcessGeneratedEvent(
        ErrorEvent::Make(e.Time(), errors::kFreePlaceAvailable));
    return;
  }

  if (wait_queue_.size() > cfg_.table_count) {
    clients_.erase(e.Name());
    ProcessGeneratedEvent(ClientEvent::Make(
        e.Time(), static_cast<int>(IdGenerated::kClientLeft), e.Name()));
    return;
  }

  auto ticket = wait_queue_.insert(wait_queue_.end(), it->first);
  clients_[e.Name()].GiveTicket(ticket);
}

void ComputerClub::ProcessClientLeft(const ClientEvent& e) {
  if (clients_.find(e.Name()) == clients_.cend()) {
    ProcessGeneratedEvent(ErrorEvent::Make(e.Time(), errors::kUknownClient));
    return;
  }

  const auto& client = clients_[e.Name()];
  if (auto status = client.Status(); status != ClientInfo::Status::Playing) {
    if (status == ClientInfo::Status::Waiting) {
      wait_queue_.erase(client.Ticket());
    }
    clients_.erase(e.Name());
    return;
  }

  auto table = client.TableNumber();
  FreeTable(table, e.Time());
  clients_.erase(e.Name());

  if (!wait_queue_.empty()) {
    auto next_client = wait_queue_.front();
    wait_queue_.pop_front();

    auto next_it = clients_.find(next_client);
    assert(next_it != clients_.cend());

    next_it->second.Sit(table);
    OccupyTable(table, e.Time());
    ProcessGeneratedEvent(
        TableEvent::Make(e.Time(), static_cast<int>(IdGenerated::kClientSat),
                         next_it->first, table));
  }
}

void ComputerClub::ValidateEvent(const Event& e) {
  auto t = e.Time();
  if (auto last_event_time = logger_.LastEventTime();
      last_event_time.has_value() &&
      !utils::time::Interval(last_event_time.value(), cfg_.working_hours.End())
           .Contains(t)) {
    throw std::invalid_argument(
        "Event timestamps must be in ascending order: " +
        utils::time::ToString(t));
  }
  if (static_cast<IdIncoming>(e.Id()) == IdIncoming::kClientSatDown &&
      static_cast<const TableEvent&>(e).TableNumber() > cfg_.table_count) {
    throw std::invalid_argument("There are only " +
                                std::to_string(cfg_.table_count) + "tables");
  }
}

}  // namespace club

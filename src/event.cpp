#include "club/event.h"

#include <regex>

#include "utils/exceptions.h"

using namespace utils;

namespace club::events {

std::unique_ptr<Event> FromString(const std::string& line) {
  std::istringstream ss(line);

  time::Time t;
  std::string hh_mm;
  if (!(ss >> hh_mm)) {
    throw utils::BadFormat(line);
  }
  try {
    t = time::FromString(hh_mm);
  } catch (...) {
    throw utils::BadFormat(line);
  }

  int id;
  if (!(ss >> id)) {
    throw utils::BadFormat(line);
  }

  std::string client_name;
  static const auto username_re = std::regex(R"(^([\w-]+)$)");
  if (!(ss >> client_name) || !std::regex_match(client_name, username_re)) {
    throw utils::BadFormat(line);
  }

  switch (static_cast<IdIncoming>(id)) {
    using enum IdIncoming;
    case kClientArrived:
    case kClientWaiting:
    case kClientLeft:
      return ClientEvent::Make(t, id, client_name);
    case kClientSatDown:
      int table_number;
      if (!(ss >> table_number)) {
        throw utils::BadFormat(line);
      }
      return TableEvent::Make(t, id, client_name, table_number);
    default:
      throw utils::BadFormat(line);
  }
}

}  // namespace club::events

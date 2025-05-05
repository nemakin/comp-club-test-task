#include "club/config.h"

#include <cassert>

#include "utils/exceptions.h"
#include "utils/time.h"

namespace club {

Config Config::Parse(std::istream& is) {
  is >> std::ws;
  std::string line;

  auto parse_int = [&](auto& value) {
    std::getline(is, line);
    size_t pos;
    try {
      value = std::stoi(line, &pos);
    } catch (...) {
      throw utils::BadFormat(line);
    }
    if (pos != line.size() || value <= 0) {
      throw utils::BadFormat(line);
    }
  };

  int table_count;
  parse_int(table_count);

  std::getline(is, line);
  utils::time::Interval working_hours;
  std::string t1, t2;
  if (std::istringstream ss(line); !(ss >> t1 >> t2)) {
    throw utils::BadFormat(line);
  }
  try {
    working_hours = utils::time::Interval(t1, t2);
  } catch (...) {
    throw utils::BadFormat(line);
  }

  int cost_per_hour;
  parse_int(cost_per_hour);

  return Config{
      static_cast<size_t>(table_count),
      working_hours,
      cost_per_hour,
  };
}

}  // namespace club

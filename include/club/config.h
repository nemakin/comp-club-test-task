#pragma once

#include "utils/time.h"

namespace club {

struct Config {
  size_t table_count;
  utils::time::Interval working_hours;
  int cost_per_hour;

  static Config Parse(std::istream& is);
};

}  // namespace club

#include "utils/time.h"

#include <gtest/gtest.h>

#include <algorithm>

namespace {

using namespace utils::time;

TEST(TestStringToTime, BackAndForth) {
  EXPECT_EQ(FromString("00:00"), 0h + 0min);
  EXPECT_EQ(FromString("23:59"), 23h + 59min);
  EXPECT_EQ(FromString("13:40"), 13h + 40min);
  EXPECT_EQ(ToString(Time(630)), "10:30");
  EXPECT_EQ(ToString(Time(1440)), "00:00");
  EXPECT_EQ(ToString(Time(1530)), "01:30");
}

TEST(TimeIntervalTest, BoundCheck) {
  Interval i("19:30", "05:00");
  std::vector<std::string> timestamps{"19:30", "21:00", "22:00", "23:59", "23:59", "01:01", "04:30", "05:00"};
  EXPECT_TRUE(std::ranges::all_of(timestamps, [&](const auto& s) { return i.Contains(FromString(s)); }));
}

TEST(TimeIntervalTest, AscendingSequence) {
  Time lower_bound = FromString("18:30");
  Time upper_bound = FromString("05:00");
  for (const auto str : {"20:00", "23:59", "03:32", "04:49"}) {
    auto t = FromString(str);
    EXPECT_TRUE(Interval(lower_bound, upper_bound).Contains(t));
    lower_bound = t;
  }
}

TEST(TimeIntervalTest, AscendingSequenceError) {
  Time lower_bound = FromString("18:30");
  Time upper_bound = FromString("05:00");
  for (size_t i = 0; const auto str : {"20:00", "23:59", "03:32", "21:10", "04:49"}) {
    auto t = FromString(str);
    if (i != 3) {
      EXPECT_TRUE(Interval(lower_bound, upper_bound).Contains(t));
    }
    lower_bound = t;
    ++i;
  }
}

}  // namespace

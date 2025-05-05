#include <gtest/gtest.h>

#include <sstream>

#include "club/config.h"
#include "utils/exceptions.h"
#include "utils/time.h"

using namespace club;

namespace {

struct ParamValid {
  std::string input;
  Config expected;
};

class TestValidConfigParsing : public ::testing::TestWithParam<ParamValid> {};

struct ParamInvalid {
  std::string input;
  std::string bad_line;
};

class TestInvalidConfigParsing : public ::testing::TestWithParam<ParamInvalid> {
};

TEST_P(TestInvalidConfigParsing, MustFail) {
  std::istringstream in(GetParam().input);

  EXPECT_THROW(
      {
        try {
          Config::Parse(in);
        } catch (utils::BadFormat& e) {
          EXPECT_STREQ(e.what(), GetParam().bad_line.c_str());
          throw;
        }
      },
      utils::BadFormat);
}

TEST_P(TestValidConfigParsing, MustSucceed) {
  Config parsed;
  std::istringstream in(GetParam().input);

  EXPECT_NO_THROW({ parsed = Config::Parse(in); });

  const auto& expected = GetParam().expected;
  EXPECT_EQ(parsed.table_count, expected.table_count);
  EXPECT_EQ(parsed.working_hours, expected.working_hours);
  EXPECT_EQ(parsed.cost_per_hour, expected.cost_per_hour);
}

INSTANTIATE_TEST_SUITE_P(
    InvalidInput, TestInvalidConfigParsing,
    ::testing::Values(ParamInvalid{"", ""}, ParamInvalid{"abc", "abc"},
                      ParamInvalid{"\n\n\n", ""},
                      ParamInvalid{"10\n09:30 18:45\n", ""},
                      ParamInvalid{
                          R"(
1e100
09:30 18:45
300
)",
                          "1e100"},

                      ParamInvalid{
                          R"(
10
09:00 24:00
300
)",
                          "09:00 24:00"},

                      ParamInvalid{
                          R"(
10
09:00 23:59
-300
)",
                          "-300"}));

INSTANTIATE_TEST_SUITE_P(
    ValidInput, TestValidConfigParsing,
    ::testing::Values(
        ParamValid{
            R"(
10
09:30 18:45
300
)",
            Config{
                .table_count = 10,
                .working_hours = utils::time::Interval("09:30", "18:45"),
                .cost_per_hour = 300,
            }},
        ParamValid{
            R"(
1
21:00 06:00
3000
)",
            Config{
                .table_count = 1,
                .working_hours = utils::time::Interval("21:00", "06:00"),
                .cost_per_hour = 3000,
            }}));

}  // namespace

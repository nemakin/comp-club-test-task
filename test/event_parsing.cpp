#include <gtest/gtest.h>

#include <memory>

#include "club/event.h"
#include "gtest/gtest.h"
#include "utils/exceptions.h"
#include "utils/time.h"

using namespace club;

namespace {
using events::EventPtr;
using ParamInvalid = std::string;

class TestInvalidEventParsing : public ::testing::TestWithParam<ParamInvalid> {
};

struct ParamValid {
  std::string input;
  std::function<EventPtr()> event_factory;
};

class TestValidEventParsing : public ::testing::TestWithParam<ParamValid> {};

TEST_P(TestInvalidEventParsing, MustFail) {
  EXPECT_THROW(
      {
        try {
          club::events::FromString(GetParam());
        } catch (utils::BadFormat& e) {
          EXPECT_STREQ(e.what(), GetParam().c_str());
          throw;
        }
      },
      utils::BadFormat);
}

TEST_P(TestValidEventParsing, MustSucceed) {
  std::unique_ptr<const club::events::Event> parsed;
  EXPECT_NO_THROW({ parsed = club::events::FromString(GetParam().input); });

  const auto& factory = GetParam().event_factory;
  EXPECT_EQ(parsed->ToString(), factory()->ToString());
}

INSTANTIATE_TEST_SUITE_P(
    InvalidInput, TestInvalidEventParsing,
    ::testing::Values(ParamInvalid{""}, ParamInvalid{"abc"},
                      ParamInvalid{"09:03"}, ParamInvalid{"09:03 1"},
                      ParamInvalid{"09:99 1 invalid-timestamp"},
                      ParamInvalid{"09:09 5 uknown_id-five"},
                      ParamInvalid{"09:09 4 non-valid-usern@me"}));

#define CLIENT_EVENT_FACTORY(t, id, name)                                   \
  ([]() {                                                                   \
    return events::ClientEvent::Make(utils::time::FromString(t), id, name); \
  })

#define TABLE_EVENT_FACTORY(t, id, name, table)                           \
  ([]() {                                                                 \
    return events::TableEvent::Make(utils::time::FromString(t), id, name, \
                                    table);                               \
  })

INSTANTIATE_TEST_SUITE_P(
    ValidInput, TestValidEventParsing,
    ::testing::Values(
        ParamValid{"11:10 1 client_1",
                   CLIENT_EVENT_FACTORY("11:10", 1, "client_1")},
        ParamValid{"13:37 2 client_2 13",
                   TABLE_EVENT_FACTORY("13:37", 2, "client_2", 13)},
        ParamValid{"16:51 3 client_3",
                   CLIENT_EVENT_FACTORY("16:51", 3, "client_3")},
        ParamValid{"00:00 4 client_4",
                   CLIENT_EVENT_FACTORY("00:00", 4, "client_4")}));

}  // namespace

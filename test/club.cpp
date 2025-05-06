#include "club/club.h"

#include <gtest/gtest.h>

#include <memory>
#include <sstream>

#include "club/config.h"
#include "club/event.h"
#include "club/logger.h"
#include "gtest/gtest.h"
#include "utils/time.h"

namespace {

struct Params {
  club::Config config;
  std::vector<std::string> events;
  std::string log_output;
};

class ComputerClubTest : public ::testing::TestWithParam<Params> {
 protected:
  void SetUp() override { club = std::make_unique<club::ComputerClub>(GetParam().config, club::EventLogger(log_os)); }

  void ProcessEvents(club::ComputerClub& club) {
    for (const auto& event_str : GetParam().events) {
      std::unique_ptr<club::events::Event> event;
      EXPECT_NO_THROW(event = club::events::FromString(event_str));
      club.ProcessIncomingEvent(std::move(event));
    }
  }

  void TearDown() override {
    club.reset();
    EXPECT_EQ(log_os.str(), GetParam().log_output.substr(1));
  }
  std::unique_ptr<club::ComputerClub> club;
  std::ostringstream log_os;
};

class ComputerClubFailingTest : public ComputerClubTest {
 protected:
};

TEST_P(ComputerClubTest, TestWorkflow) { EXPECT_NO_THROW(ProcessEvents(*club)); }

INSTANTIATE_TEST_SUITE_P(ShouldSucceed, ComputerClubTest,
                         ::testing::Values(
                             Params{
                                 club::Config{1, utils::time::Interval("08:00", "21:00"), 10},
                                 {
                                     "08:00 1 client1",
                                     "11:00 2 client1 1",
                                 },
                                 R"(
08:00
08:00 1 client1
11:00 2 client1 1
21:00 11 client1
21:00
1 100 10:00
)",
                             },
                             Params{club::Config{3, utils::time::Interval("09:00", "19:00"), 10},
                                    {
                                        "08:48 1 client1",
                                        "09:41 1 client1",
                                        "09:48 1 client2",
                                        "09:52 3 client1",
                                        "09:54 2 client1 1",
                                        "10:25 2 client2 2",
                                        "10:58 1 client3",
                                        "10:59 2 client3 3",
                                        "11:30 1 client4",
                                        "11:35 2 client4 2",
                                        "11:45 3 client4",
                                        "12:33 4 client1",
                                        "12:43 4 client2",
                                        "15:52 4 client4",
                                    },
                                    R"(
09:00
08:48 1 client1
08:48 13 NotOpenYet
09:41 1 client1
09:48 1 client2
09:52 3 client1
09:52 13 ICanWaitNoLonger!
09:54 2 client1 1
10:25 2 client2 2
10:58 1 client3
10:59 2 client3 3
11:30 1 client4
11:35 2 client4 2
11:35 13 PlaceIsBusy
11:45 3 client4
12:33 4 client1
12:33 12 client4 1
12:43 4 client2
15:52 4 client4
19:00 11 client3
19:00
1 70 05:58
2 30 02:18
3 90 08:01
)"}));

}  // namespace

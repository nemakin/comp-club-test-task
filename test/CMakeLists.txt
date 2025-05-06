FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/90a41521142c978131f38c6da07b4eb96a9f1ff6.zip
  FIND_PACKAGE_ARGS NAMES GTest
)
FetchContent_MakeAvailable(googletest)

add_executable(tests time.cpp config_parsing.cpp event_parsing.cpp club.cpp)
target_link_libraries(tests PRIVATE club GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

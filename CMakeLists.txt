cmake_minimum_required(VERSION 3.24)
project(comp-club-test-task LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

include(CTest)

add_subdirectory(src)

if (BUILD_TESTING)
  add_subdirectory(test)
endif()

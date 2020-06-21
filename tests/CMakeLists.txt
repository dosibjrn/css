cmake_minimum_required(VERSION 3.10)
find_package(gtest QUIET)

if (GTEST_FOUND)
  message(STATUS "gtest found, building tests")
  add_executable(cssLibTests test_set_bonus_calculus.cpp
  main.cpp)
  target_link_libraries(cssLibTests PRIVATE cssLib gtest)
else()
  message(STATUS "No gtest found, skipping tests")
endif()

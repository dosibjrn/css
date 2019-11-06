cmake_minimum_required(VERSION 3.10)

file(GLOB header_list ${CMAKE_CURRENT_SOURCE_DIR} *.h)
file(GLOB source_list ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
# message(STATUS "headers: ${header_list}, sources: ${source_list}")
add_executable(css ${header_list} ${source_list})

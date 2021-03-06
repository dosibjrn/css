cmake_minimum_required(VERSION 3.10)
project(css LANGUAGES CXX)

set(header_list
  assumptions.h
  combat.h
  cooldown.h
  create_sample_outputs.sh
  csv.h
  dps.h
  fight_result.h
  healtarget.h
  hps.h
  item.h
  item_operations.h
  item_picker.h
  item_picking_main.h
  item_table.h
  leveling.h
  log_entry.h
  mob.h
  parse_based.h
  priest_character.h
  priest_talents.h
  pve.h
  pvp.h
  regen.h
  school.h
  set_bonus_calculus.h
  set_bonuses.h
  spell.h
  spell_priest_modify.h
  spell_sequence_priest.h
  spells_priest_disc.h
  spells_priest_holy.h
  spells_priest_shadow.h
  stats.h
  wand.h
  water.h
  )

set(source_list
  assumptions.cpp
  combat.cpp
  cooldown.cpp
  csv.cpp
  dps.cpp
  fight_result.cpp
  hps.cpp
  item_operations.cpp
  item_picker.cpp
  item_picking_main.cpp
  item_table.cpp
  leveling.cpp
  log_entry.cpp
  parse_based.cpp
  priest_character.cpp
  pve.cpp
  pvp.cpp
  set_bonus_calculus.cpp
  set_bonuses.cpp
  spell_priest_modify.cpp
  spell_sequence_priest.cpp
  spells_priest_disc.cpp
  spells_priest_holy.cpp
  spells_priest_shadow.cpp
  stats.cpp
  )

if (UNIX)
  add_compile_options(-Wall)
endif()

# message(STATUS "headers: ${header_list}, sources: ${source_list}")
find_package(OpenMP REQUIRED)
# find_package(Eigen REQUIRED)



add_library(cssLib ${header_list} ${source_list})
target_link_libraries(cssLib PUBLIC OpenMP::OpenMP_CXX)
# TODO: an actual include dir
target_include_directories(cssLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cssLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/eigen3)

add_subdirectory(tests)

set(exec_name css)
add_executable(${exec_name} main.cpp)
target_link_libraries(${exec_name} PRIVATE cssLib)

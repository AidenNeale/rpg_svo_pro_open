cmake_minimum_required(VERSION 3.8)
project(rpg_common)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)

file(GLOB_RECURSE HEADERS "include/*")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include)

ament_export_include_directories(include)
ament_export_targets(export_${PROJECT_NAME})

ament_package()
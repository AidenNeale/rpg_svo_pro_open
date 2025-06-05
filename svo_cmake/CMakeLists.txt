cmake_minimum_required(VERSION 3.5)
project(svo_cmake)

find_package(ament_cmake REQUIRED)

# Configure environment hook files from templates
configure_file(
  cmake/svo_cmake-extras.cmake.develspace.in
  ${CMAKE_CURRENT_BINARY_DIR}/svo_cmake-extras.cmake
  @ONLY
)

# Install the generated environment hook
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/svo_cmake-extras.cmake
  DESTINATION share/${PROJECT_NAME}/cmake
)

ament_environment_hooks(
  "${CMAKE_CURRENT_BINARY_DIR}/svo_cmake-extras.cmake"
)

install(DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}/cmake
)

ament_package()
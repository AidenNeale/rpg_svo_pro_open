cmake_minimum_required(VERSION 3.5)
project(svo_cmake)

find_package(ament_cmake REQUIRED)

# Install cmake modules
install(DIRECTORY cmake/Modules
  DESTINATION share/${PROJECT_NAME}/cmake
)

# Install the extras file
install(FILES
  cmake/svo_cmake-extras.cmake
  DESTINATION share/${PROJECT_NAME}/cmake
)

ament_package(CONFIG_EXTRAS cmake/svo_cmake-extras.cmake)
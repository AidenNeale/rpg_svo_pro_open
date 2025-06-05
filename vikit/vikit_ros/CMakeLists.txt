cmake_minimum_required(VERSION 3.8)
project(vikit_ros)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)

find_package(rclcpp REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

find_package(vikit_common REQUIRED)

set(HEADERS
  include/vikit/img_type_conversion.h
  include/vikit/output_helper.h
  include/vikit/params_helper.h
  )

set(SOURCES
  src/output_helper.cpp
  src/img_type_conversion.cpp
  )

# Create vikit library
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  cv_bridge
  tf2_ros
  tf2_geometry_msgs
  visualization_msgs
  geometry_msgs
  sensor_msgs
  vikit_common
)
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)

# Install library and headers
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/
  DESTINATION include
)

ament_export_dependencies(tf2_ros tf2_geometry_msgs)
ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)
ament_package()

cmake_minimum_required(VERSION 3.8)
project(svo_common)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)

find_package(vikit_cameras REQUIRED)
find_package(vikit_common REQUIRED)


set(HEADERS
  include/svo/common/camera.h
  include/svo/common/camera_fwd.h
  include/svo/common/feature_wrapper.h
  include/svo/common/frame.h
  include/svo/common/imu_calibration.h
  include/svo/common/logging.h
  include/svo/common/occupancy_grid_2d.h
  include/svo/common/point.h
  include/svo/common/seed.h
  include/svo/common/transformation.h
  include/svo/common/types.h
  )

set(SOURCES
  src/frame.cpp
  src/point.cpp
  )

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  vikit_cameras
  vikit_common
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} yaml-cpp Eigen3::Eigen)

################################################################################
# TESTS

################################################################################
###
### GTEST
###

ament_add_gtest(${PROJECT_NAME}_tests
  test/test_main.cpp
  test/test_feature_wrapper.cpp
)
# ament_target_dependencies(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME})

ament_add_gtest(${PROJECT_NAME}_container_helpers_tests
  test/test_container_helpers.cpp
)
# ament_target_dependencies(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_container_helpers_tests ${PROJECT_NAME})


################################################################################

# Install library and headers
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/
  DESTINATION include/
)

ament_export_dependencies(rclcpp vikit_cameras vikit_common OpenCV)
ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)
ament_package()

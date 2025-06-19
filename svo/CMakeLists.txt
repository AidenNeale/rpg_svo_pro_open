cmake_minimum_required(VERSION 3.8)
project(svo)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(opengv REQUIRED)

find_package(rpg_common REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_direct REQUIRED)
find_package(svo_img_align REQUIRED)
find_package(svo_online_loopclosing)
find_package(svo_pgo REQUIRED)
find_package(svo_test_utils REQUIRED)
find_package(svo_tracker REQUIRED)
find_package(svo_vio_common REQUIRED)
find_package(vikit_cameras REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_ros REQUIRED)
find_package(vikit_solver REQUIRED)


list(APPEND SVO_SOURCEFILES
  src/frame_handler_mono.cpp
  src/frame_handler_stereo.cpp
  src/frame_handler_array.cpp
  src/frame_handler_base.cpp
  src/map.cpp
  src/pose_optimizer.cpp
  src/initialization.cpp
  src/reprojector.cpp
  src/imu_handler.cpp
  src/stereo_triangulation.cpp
  src/io.cpp
)

list(APPEND SVO_HEADERFILES
  include/svo/abstract_bundle_adjustment.h
  include/svo/frame_handler_base.h
  include/svo/frame_handler_mono.h
  include/svo/frame_handler_stereo.h
  include/svo/frame_handler_array.h
  include/svo/global.h
  include/svo/imu_handler.h
  include/svo/initialization.h
  include/svo/map.h
  include/svo/pose_optimizer.h
  include/svo/reprojector.h
  include/svo/stereo_triangulation.h
  include/svo/io.h
  include/svo/svo.h
)

################################################################################
# Create svo library
add_library(${PROJECT_NAME} SHARED ${SVO_SOURCEFILES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  opengv
  rpg_common
  svo_common
  svo_direct
  svo_img_align
  svo_online_loopclosing
  svo_pgo
  svo_test_utils
  svo_tracker
  svo_vio_common
  vikit_cameras
  vikit_common
  vikit_ros
  vikit_solver
)
target_link_libraries(svo ${OpenCV_LIBRARIES} opengv Eigen3::Eigen)

################################################################################
###
### GTEST
###

ament_add_gtest(${PROJECT_NAME}-test-frame test/test_frame.cpp)
target_link_libraries(${PROJECT_NAME}-test-frame ${PROJECT_NAME})

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

ament_export_dependencies(
  rclcpp
  rpg_common
  svo_common
  svo_direct
  svo_img_align
  svo_test_utils
  svo_tracker
  svo_vio_common
  vikit_cameras
  vikit_common
  vikit_ros
  vikit_solver
)
ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)
ament_package()


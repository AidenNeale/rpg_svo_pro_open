cmake_minimum_required (VERSION 3.8)
project(svo_test_utils)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)


find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(OpenCV REQUIRED)

find_package(vikit_common REQUIRED)
find_package(vikit_cameras REQUIRED)
find_package(vikit_ros REQUIRED)
find_package(svo_common REQUIRED)

################################################################################
set(HEADERS
  include/svo/test_utils/test_utils.h
  include/svo/test_utils/imu_simulation.h
  include/svo/test_utils/synthetic_dataset.h
  include/svo/test_utils/simple_raytracer.h
  include/svo/test_utils/trajectory_generator.h
  include/svo/test_utils/point_cloud_generator.h
)

set(SOURCES
  src/test_utils.cpp
  src/imu_simulation.cpp
  src/synthetic_dataset.cpp
  src/simple_raytracer.cpp
  src/trajectory_generator.cpp
  src/point_cloud_generator.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME} svo_common vikit_common vikit_cameras vikit_ros ament_index_cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} yaml-cpp Eigen3::Eigen)


################################################################################
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/
  DESTINATION include/
)

ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)
ament_package()

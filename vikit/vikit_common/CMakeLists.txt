cmake_minimum_required(VERSION 3.8)
project(vikit_common)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GTest REQUIRED)
find_package(Eigen3 REQUIRED)

set(HEADERS
  include/vikit/blender_utils.h
  include/vikit/math_utils.h
  include/vikit/performance_monitor.h
  include/vikit/ringbuffer.h
  include/vikit/sample.h
  include/vikit/timer.h
  include/vikit/user_input_thread.h
  include/vikit/vision.h
  include/vikit/homography_decomp.h
  include/vikit/homography.h
  include/vikit/path_utils.h
  include/vikit/csv_utils.h
  include/aslam/common/entrypoint.h
  include/aslam/common/macros.h
  include/aslam/common/memory.h
  include/aslam/common/pose-types.h
  include/aslam/common/yaml-serialization.h
  include/aslam/common/yaml-serialization-eigen.h
  )

set(SOURCES
  src/homography.cpp
  src/math_utils.cpp
  src/performance_monitor.cpp
  src/sample.cpp
  src/user_input_thread.cpp
  src/vision.cpp
  )


# Create vikit shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    /local/include # For Minkindr
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBRARIES} Eigen3::Eigen)


# Unit tests
ament_add_gtest(homography test/test_homography.cpp)
target_link_libraries(homography GTest::gtest ${PROJECT_NAME})

ament_add_gtest(math_utils test/test_math_utils.cpp)
target_link_libraries(math_utils GTest::gtest ${PROJECT_NAME})

# Other tests (executables)
add_executable(vikit_test_timer test/test_timer.cpp)
target_link_libraries(vikit_test_timer ${PROJECT_NAME})

add_executable(vikit_test_common test/test_common.cpp)
target_link_libraries(vikit_test_common ${PROJECT_NAME})

install(TARGETS vikit_test_timer vikit_test_common
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

# Install library and headers
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/
  DESTINATION include
)

ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)
ament_export_dependencies(Eigen3)

ament_package()

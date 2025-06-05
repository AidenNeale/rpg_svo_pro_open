cmake_minimum_required(VERSION 3.8)
project(svo_direct)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(fast REQUIRED)
find_package(glog REQUIRED)

find_package(svo_common REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_cameras REQUIRED)
find_package(vikit_solver REQUIRED)
find_package(svo_test_utils REQUIRED)

set(HEADERS
  include/svo/direct/patch_warp.h
  include/svo/direct/patch_score.h
  include/svo/direct/patch_utils.h
  include/svo/direct/feature_alignment.h
  include/svo/direct/feature_detection_types.h
  include/svo/direct/feature_detection.h
  include/svo/direct/feature_detection_utils.h
  include/svo/direct/matcher.h
  include/svo/direct/depth_filter.h
  include/svo/direct/depth_estimation.h
  include/svo/direct/elder_zucker.h
)

set(SOURCES
  src/patch_warp.cpp
  src/feature_alignment.cpp
  src/feature_detection.cpp
  src/feature_detection_utils.cpp
  src/matcher.cpp
  src/depth_filter.cpp
  src/depth_estimation.cpp
  src/elder_zucker.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  svo_common
  svo_test_utils
  vikit_common
  vikit_solver
  fast
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Eigen3::Eigen glog::glog)

################################################################################
###
### Executables
###
add_executable(svo_test_patch_score test/test_patch_score.cpp)
target_link_libraries(svo_test_patch_score ${PROJECT_NAME})
ament_target_dependencies(svo_test_patch_score
  ament_index_cpp
)

add_executable(svo_test_edgelet_direction test/test_edgelet_direction.cpp)
target_link_libraries(svo_test_edgelet_direction
  ${PROJECT_NAME}
)
ament_target_dependencies(svo_test_edgelet_direction
  ament_index_cpp
)

add_executable(svo_test_edgelet_extraction test/test_edgelet_extraction.cpp)
target_link_libraries(svo_test_edgelet_extraction ${PROJECT_NAME})
ament_target_dependencies(svo_test_edgelet_extraction
  ament_index_cpp
)

add_executable(svo_test_feature_alignment test/test_feature_alignment.cpp)
target_link_libraries(svo_test_feature_alignment ${PROJECT_NAME})
ament_target_dependencies(svo_test_feature_alignment
  ament_index_cpp
)
################################################################################
###
### GTEST
###
ament_add_gtest(${PROJECT_NAME}_test_depth_estimation
  test/test_depth_estimation.cpp
  test/test_utils.h
)
target_link_libraries(${PROJECT_NAME}_test_depth_estimation ${PROJECT_NAME})
ament_target_dependencies(${PROJECT_NAME}_test_depth_estimation
  ament_index_cpp
)

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

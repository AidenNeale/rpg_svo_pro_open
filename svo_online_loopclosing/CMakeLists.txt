cmake_minimum_required(VERSION 3.8)
project(svo_online_loopclosing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(DBoW2 REQUIRED)
find_package(opengv REQUIRED)

find_package(vikit_common REQUIRED)
find_package(vikit_cameras REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_pgo REQUIRED)
find_package(rpg_common REQUIRED)


set(HEADERS
  include/svo/online_loopclosing/bow.h
  include/svo/online_loopclosing/geometric_verification.h
  include/svo/online_loopclosing/loop_closing.h
  include/svo/online_loopclosing/read_file.h
  include/svo/online_loopclosing/keyframe.h
  include/svo/online_loopclosing/loop_closing_types.h
  include/svo/online_loopclosing/map_alignment.h
  )

set(SOURCES
  src/bow.cpp
  src/geometric_verification.cpp
  src/loop_closing.cpp
  src/read_file.cpp
  src/map_alignment.cpp
  )

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME} rclcpp DBoW2 opengv svo_common svo_pgo rpg_common vikit_common vikit_cameras)
target_link_libraries(${PROJECT_NAME} opengv ${OpenCV_LIBRARIES} Eigen3::Eigen)

add_library(bow src/bow.cpp)
target_include_directories(bow
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(bow DBoW2 opengv svo_common vikit_common)
target_link_libraries(bow ${OpenCV_LIBRARIES} opengv Eigen3::Eigen)

add_executable(createvoc test/createvoc.cpp)
ament_target_dependencies(createvoc ament_index_cpp)
target_link_libraries(createvoc bow)

################################################################################

# Install library and headers
install(TARGETS ${PROJECT_NAME} bow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/
  DESTINATION include/
)
install(DIRECTORY vocabularies
  DESTINATION share/${PROJECT_NAME}/
)

ament_export_libraries(${PROJECT_NAME} bow)
ament_export_include_directories(include)
ament_export_dependencies(
  svo_pgo
  opengv
  DBoW2
)
ament_package()

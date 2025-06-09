cmake_minimum_required(VERSION 3.8)
project(svo_ros)

ADD_DEFINITIONS(-DSVO_LOOP_CLOSING)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(image_transport REQUIRED)
find_package(PCL COMPONENTS common REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(Ceres REQUIRED)

find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

find_package(rpg_common REQUIRED)
find_package(svo REQUIRED)
find_package(svo_ceres_backend REQUIRED)
find_package(svo_common REQUIRED)
find_package(svo_msgs REQUIRED)
find_package(svo_online_loopclosing REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_ros REQUIRED)

# set(USE_GTSAM false)
# include(SvoSetup)
# if(USE_GLOBAL_MAP OR USE_GTSAM)
#   find_package(GTSAMCMakeTools REQUIRED)
#   find_package(GTSAM REQUIRED)
#   set(GTSAM_LIBRARIES "gtsam")
# endif()
# message(STATUS "GTSAM include dir: ${GTSAM_INCLUDE_DIR}")
# message(STATUS "GTSAM library: ${GTSAM_LIBRARIES}")
# include_directories(${GTSAM_INCLUDE_DIR})


set(HEADERS
  include/svo_ros/svo_interface.h
  include/svo_ros/svo_factory.h
  include/svo_ros/visualizer.h
  include/svo_ros/svo_nodelet.h
  include/svo_ros/csv_dataset_reader.h
  include/svo_ros/ceres_backend_factory.h
)

set(SOURCES
  src/ceres_backend_factory.cpp
  src/svo_interface.cpp
  src/svo_factory.cpp
  src/svo_node_base.cpp
  src/visualizer.cpp
  src/csv_dataset_reader.cpp
)

# not used anymore
# if(USE_GTSAM)
#     ADD_DEFINITIONS(-DSVO_USE_GTSAM_BACKEND)
#     list(APPEND HEADERS
#       include/svo_ros/backend_visualizer.h
#       include/svo_ros/backend_factory.h)
#     list(APPEND SOURCES
#       src/backend_visualizer.cpp
#       src/backend_factory.cpp)
# endif()

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  ament_index_cpp
  rclcpp
  cv_bridge
  image_transport
  PCL
  pcl_conversions
  geometry_msgs
  nav_msgs
  sensor_msgs
  visualization_msgs
  rpg_common
  svo
  svo_ceres_backend
  svo_common
  svo_msgs
  svo_online_loopclosing
  vikit_common
  vikit_ros
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} Eigen3::Eigen ceres)

add_executable(svo_benchmark src/benchmark_node.cpp)
target_link_libraries(svo_benchmark ${PROJECT_NAME})

add_executable(svo_node src/svo_node.cpp)
target_include_directories(svo_node
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(svo_node ${PROJECT_NAME})

add_library(svo_nodelet SHARED src/svo_nodelet.cpp)
target_include_directories(svo_nodelet
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(svo_nodelet ${PROJECT_NAME})
ament_target_dependencies(svo_nodelet rclcpp rclcpp_components)
rclcpp_components_register_nodes(svo_nodelet "svo::SvoNodelet")

################################################################################

install(TARGETS ${PROJECT_NAME} svo_node svo_nodelet
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY include/
  DESTINATION include/
)
install(
  DIRECTORY param scripts config launch
  DESTINATION share/${PROJECT_NAME}/
)
ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)
ament_package()



cmake_minimum_required(VERSION 3.8)
project(svo_msgs)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  msg/DenseInput.msg
  msg/DenseInputWithFeatures.msg
  msg/Feature.msg
  msg/Info.msg
  msg/NbvTrajectory.msg
  DEPENDENCIES geometry_msgs sensor_msgs std_msgs
)

ament_export_dependencies(rosidl_default_runtime geometry_msgs sensor_msgs std_msgs)

ament_package()
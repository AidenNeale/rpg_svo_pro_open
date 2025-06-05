cmake_minimum_required(VERSION 3.8)
project(vikit_solver)

find_package(ament_cmake REQUIRED)

SET(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)


#############
# LIBRARIES #
#############
add_library(${PROJECT_NAME} SHARED
  src/robust_cost.cpp
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Install library
install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY include/
  DESTINATION include/
)

##########
# EXPORT #
##########
ament_export_libraries(${PROJECT_NAME})
ament_export_include_directories(include)
ament_package()

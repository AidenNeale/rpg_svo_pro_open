cmake_minimum_required (VERSION 3.8)
project(svo_vio_common)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)

find_package(svo_common REQUIRED)

#############
# LIBRARIES #
#############
set(HEADERS
  include/svo/vio_common/logging.hpp
  include/svo/vio_common/matrix_operations.hpp
  include/svo/vio_common/matrix.hpp
  include/svo/vio_common/test_utils.hpp
  include/svo/vio_common/backend_types.hpp
  )

set(SOURCES
  src/vio_common.cpp
  src/test_utils.cpp
  )

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME} svo_common)
target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)


##########
# GTESTS #
##########


##########
# EXPORT #
##########
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

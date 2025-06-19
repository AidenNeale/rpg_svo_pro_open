cmake_minimum_required(VERSION 3.8)
project(svo_img_align)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)
find_package(Eigen3 REQUIRED)

find_package(svo_common REQUIRED)
find_package(svo_direct REQUIRED)
find_package(vikit_cameras REQUIRED)
find_package(vikit_common REQUIRED)
find_package(vikit_solver REQUIRED)

set(HEADERS_CPU
  include/svo/img_align/sparse_img_align_base.h
  include/svo/img_align/sparse_img_align.h
)

set(SOURCES_CPU
  src/sparse_img_align_base.cpp
  src/sparse_img_align.cpp
  )

add_library(${PROJECT_NAME} SHARED ${SOURCES_CPU})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
ament_target_dependencies(${PROJECT_NAME}
  svo_common
  svo_direct
  vikit_cameras
  vikit_common
  vikit_solver
)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS} Eigen3::Eigen)


# Install library and headers
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
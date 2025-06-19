cmake_minimum_required (VERSION 3.8)
project(svo_pgo)

find_package(svo_cmake REQUIRED)
include(SvoSetup)

find_package(ament_cmake REQUIRED)
find_package(Ceres REQUIRED)
find_package(OpenCV REQUIRED)

find_package(svo_common REQUIRED)
find_package(vikit_common REQUIRED)

set(HEADERS
  include/svo/pgo/pgo.h
  )

set(SOURCES
  src/pgo.cpp
  )

add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${CERES_INCLUDE_DIRS}
)
ament_target_dependencies(${PROJECT_NAME} svo_common vikit_common)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} ${CERES_LIBRARIES} Eigen3::Eigen)


add_executable(test_pgo
  test/test.cpp
)
target_link_libraries(test_pgo ${PROJECT_NAME})

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

if(svo_cmake_DIR)
  # In install space
  list(INSERT CMAKE_MODULE_PATH 0 "${svo_cmake_DIR}/../cmake/Modules")
else()
  # In devel space
  list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}/Modules")
endif()

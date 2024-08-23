#**********************************
#  Created by boil on 2022/10/24.
#**********************************
#|--------------------------------
#| rendu-core examples project
#|--------------------------------
#if (RD_BUILD_EXAMPLES)
  set(PROJECT_NAME example)
  rendu_add_subdirectory(
      ${CMAKE_CURRENT_SOURCE_DIR}
      # Exclude
      ${CMAKE_CURRENT_SOURCE_DIR}/boost
      ${CMAKE_CURRENT_SOURCE_DIR}/coroutine
      ${CMAKE_CURRENT_SOURCE_DIR}/cppcoro
      ${CMAKE_CURRENT_SOURCE_DIR}/entt
      ${CMAKE_CURRENT_SOURCE_DIR}/fmt
      ${CMAKE_CURRENT_SOURCE_DIR}/protobuf
#      ${CMAKE_CURRENT_SOURCE_DIR}/spdlog
      ${CMAKE_CURRENT_SOURCE_DIR}/zltoolkit
  )
#endif ()



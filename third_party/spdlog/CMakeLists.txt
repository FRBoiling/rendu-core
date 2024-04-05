#**********************************
#  Created by boil on 2023/4/9.
#**********************************

get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/spdlog_config.h.in.cmake")
  configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/spdlog_config.h.in.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/include/spdlog/spdlog_config.h"
      @ONLY)
endif ()

if(WIN32)
  list(APPEND EXCLUDE_FILES
      "src/details/os_unix.cpp"
      "include/spdlog/details/tcp_client_unix.h"
      "include/spdlog/details/udp_client_unix.h")
else()
  list(APPEND EXCLUDE_FILES
      "src/details/os_windows.cpp"
      "src/sinks/wincolor_sink.cpp"
      "include/spdlog/details/tcp_client_windows.h"
      "include/spdlog/details/udp_client_windows.h"
      "include/spdlog/details/windows_include.h"
      "include/spdlog/sinks/win_eventlog_sink.h"
      "include/spdlog/sinks/wincolor_sink.h")
endif()

rendu_add_library(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    EXCLUDE
    ${EXCLUDE_FILES}
    PROJECT
    ${PROJECT_NAME}
    NAME
    ${FOLDER_NAME}
    LINKOPTS
    rendu-dependency-interface
    DEPS
    third_party::fmt
)

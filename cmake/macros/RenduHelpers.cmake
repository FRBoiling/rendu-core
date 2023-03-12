#**********************************
#  Created by boil on 2022/10/19.
#**********************************

include(CMakeParseArguments)
include(CheckCXXSourceCompiles)



# 将所有子目录添加。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_add_subdirectory(
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/platform)
#
function(rendu_add_subdirectory _cur_dir)
  list(FIND ARGN "${_cur_dir}" _is_excluded)
  if (_is_excluded EQUAL -1)
    list(APPEND _dir ${_cur_dir})
    file(GLOB _sub_dirs ${_cur_dir}/*)
    foreach (_sub_dir ${_sub_dirs})
      if (IS_DIRECTORY ${_sub_dir})
        list(FIND ARGN "${_sub_dir}" _is_excluded)
        if (_is_excluded EQUAL -1)
          get_filename_component(_element_name ${_sub_dir} NAME)
          add_subdirectory(${_element_name})
        endif ()
      endif ()
    endforeach ()
  endif ()
endfunction(rendu_add_subdirectory)

# Parameters:
# NAME: name of target (see Note)
# SRC_DIR: src_dir
# BIN_DIR: bin_dir
# HDRS: List of public header files for the library
# SRCS: List of source files for the library
# DEPS: List of other libraries to be linked in to the binary targets
# COPTS: List of private compile options
# DEFINES: List of public defines
# LINKOPTS: List of link options
# PUBLIC: Add this so that this library will be exported under rendu::
#
# rendu_add_library(
#   NAME
#     awesome
#   HDRS
#     "a.h"
#   SRCS
#     "a.cc"
# )
#
# rendu_add_executable(
#   NAME
#     awesome_main
#   SRCS
#     "awesome_main.cc"
#   DEPS
#     rendu::awesome
#     GTest::gmock
#     GTest::gtest_main
# )
function(rendu_add_library)
  cmake_parse_arguments(RD_LIB
      ""
      "NAME"
      "SRC_DIR;BIN_DIR;HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(_NAME "${PROJECT_NAME}_${RD_LIB_NAME}")

  message(STATUS "[lib ] " ${_NAME})
  CollectSourceFiles(
      ${RD_LIB_SRC_DIR}
      PRIVATE_SRCS
      #      # Exclude
      #      ${RD_LIB_SRC_DIR}/precompiled_headers
  )
  CollectHeaderFiles(
      ${RD_LIB_SRC_DIR}
      PRIVATE_HDRS
      # Exclude
      ${RD_LIB_SRC_DIR}/precompiled_headers
  )

  list(APPEND _SRCS
      ${RD_LIB_SRCS}
      ${PRIVATE_HDRS}
      ${PRIVATE_SRCS}
      )

  list(APPEND temp_SRCS
      ${_SRCS}
      )
  foreach (src_file IN LISTS temp_SRCS)
    if (${src_file} MATCHES ".*\\.(h|inc)")
      list(REMOVE_ITEM temp_SRCS "${src_file}")
    endif ()
  endforeach ()

  if (temp_SRCS STREQUAL "")
    set(_LIB_IS_INTERFACE 1)
  else ()
    set(_LIB_IS_INTERFACE 0)
  endif ()

  if (USE_COREPCH)
    CollectHeaderFiles(
        ${RD_LIB_SRC_DIR}/precompiled_headers
        _PCH_HEADER
    )
  endif (USE_COREPCH)

  GroupSources(${RD_LIB_SRC_DIR})
  #  add_definitions(-DTRINITY_API_EXPORT_COMMON)

  CollectIncludeDirectories(
      ${RD_LIB_SRC_DIR}
      _INCLUDES
      # Exclude
      ${RD_LIB_SRC_DIR}/precompiled_headers
  )

  if (_LIB_IS_INTERFACE) #TODO:BOIL head only
    add_library(${_NAME}
        INTERFACE
        )
    target_sources(${_NAME}
        INTERFACE
        ${_SRCS}
        )

    target_link_libraries(${_NAME}
        INTERFACE
        ${RD_LIB_PRIVATE}
        ${RD_LIB_DEPS}
        )
    target_include_directories(${_NAME}
        INTERFACE
        "$<BUILD_INTERFACE:${_INCLUDES}>"
        ${RD_LIB_BIN_DIR}
        )
    target_compile_definitions(${_NAME} INTERFACE ${RD_LIB_DEFINES})

  else ()
    add_library(${_NAME})
    target_sources(${_NAME} PRIVATE ${_SRCS})
    target_link_libraries(${_NAME}
        PRIVATE
        ${RD_LIB_PRIVATE}
        PUBLIC
        ${RD_LIB_DEPS}
        )
    target_include_directories(${_NAME}
        PUBLIC
        ${_INCLUDES}
        ${RD_LIB_BIN_DIR}
        )
    target_compile_definitions(${_NAME} PUBLIC ${RD_LIB_DEFINES})
  endif ()

  #  add_dependencies(${_NAME} ${RENDU_LIB_CMAKE_BINARY_DIR}/revision_data.h)
  set_target_properties(${_NAME}
      PROPERTIES
      FOLDER
      ${PROJECT_NAME})

  if (BUILD_SHARED_LIBS)
    message(STATUS "build shared libs")
    if (UNIX)
      install(TARGETS ${_NAME}
          LIBRARY
          DESTINATION lib)
    elseif (WIN32)
      install(TARGETS ${_NAME}
          RUNTIME
          DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif ()
  endif ()

  # Generate precompiled header
  if (USE_COREPCH)
    message(STATUS "use core pch ")
    set(_header "${_PCH_HEADER}")
    if (_header STREQUAL "")
    else ()
      add_cxx_pch(${_NAME} ${_PCH_HEADER})
    endif ()
  endif ()
  add_library(${PROJECT_NAME}::${RD_LIB_NAME} ALIAS ${_NAME})
endfunction(rendu_add_library)

function(rendu_add_executable)
  cmake_parse_arguments(RD_EXEC
      ""
      "NAME"
      "SRC_DIR;BIN_DIR;HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(_NAME "${PROJECT_NAME}_${RD_EXEC_NAME}")
  message(STATUS "[exec] " ${_NAME})
  CollectAllFiles(
      ${RD_EXEC_SRC_DIR}
      _SRCS
      # Exclude
      ${RD_EXEC_SRC_DIR}/precompiled_headers
  )

  list(APPEND _SRCS
      ${RD_EXEC_SRCS}
      )

  if (USE_COREPCH)
    CollectHeaderFiles(
        ${RD_EXEC_SRCS}/precompiled_headers
        _PCH_HEADER
    )
    #    set(PRIVATE_PCH_HEADER precompiled_headers/${RENDU_EXEC_NAME}_pch.h)
  endif (USE_COREPCH)

  GroupSources(${RD_EXEC_SRC_DIR})
  add_executable(${_NAME} ${_SRCS})
  target_link_libraries(${_NAME}
      PUBLIC
      ${RD_EXEC_PRIVATE}
      ${RD_EXEC_DEPS}
      )

  CollectIncludeDirectories(
      ${RD_EXEC_SRC_DIR}
      _INCLUDES
      # Exclude
      ${RD_EXEC_SRC_DIR}/precompiled_headers)

  target_include_directories(${_NAME}
      PRIVATE
      ${_INCLUDES}
      ${RD_EXEC_BIN_DIR}
      )
  set_target_properties(${_NAME}
      PROPERTIES
      FOLDER
      ${RD_EXEC_NAME})

  if (BUILD_SHARED_EXECS)
    if (UNIX)
      install(TARGETS ${_NAME}
          LIBRARY
          DESTINATION lib)
    elseif (WIN32)
      install(TARGETS ${_NAME}
          RUNTIME
          DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif ()
  endif ()
  # Generate precompiled header
  if (USE_COREPCH)
    set(_header "${_PCH_HEADER}")
    if (_header STREQUAL "")
    else ()
      add_cxx_pch(${_NAME} ${_PCH_HEADER})
    endif ()
  endif ()
endfunction(rendu_add_executable)

function(rendu_add_test)
  cmake_parse_arguments(RENDU_TEST
      ""
      "NAME"
      "CMAKE_CUR_SOURCE_DIR;CMAKE_CUR_BINARY_DIR;CMAKE_BINARY_DIR;COPTS;DEFINES;LINKOPTS;DEPS"
      ${ARGN}
      )
  set(_NAME "${PROJECT_NAME}_${RENDU_TEST_NAME}")
  message(STATUS "[test] " ${_NAME})
  CollectAllFiles(
      ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}
      PRIVATE_SOURCES
      # Exclude
      ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
  )

  list(APPEND PRIVATE_SOURCES
      #      ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}/Debugging/Errors.cpp
      #      ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}/Debugging/Errors.h
      )

  if (USE_COREPCH)
    CollectHeaderFiles(
        ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
        _PCH_HEADER
    )
    #    set(_PCH_HEADER precompiled_headers/${RENDU_TEST_NAME}_pch.h)
  endif (USE_COREPCH)

  GroupSources(${RENDU_TEST_CMAKE_CUR_SOURCE_DIR})
  add_executable(${_NAME} "")
  target_sources(${_NAME} PRIVATE ${PRIVATE_SOURCES})
  target_link_libraries(${_NAME}
      PRIVATE
      rendu-core-interface
      PUBLIC
      ${RENDU_TEST_DEPS}
      )
  CollectIncludeDirectories(
      ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}
      PUBLIC_INCLUDES
      # Exclude
      ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}/precompiled_headers)
  target_include_directories(${_NAME}
      PUBLIC
      # Provide the binary _dir for all child targets
      ${RENDU_TEST_CMAKE_BINARY_DIR}
      ${PUBLIC_INCLUDES}
      PRIVATE
      ${RENDU_TEST_CMAKE_CUR_BINARY_DIR})
  set_target_properties(${_NAME}
      PROPERTIES
      FOLDER
      ${RENDU_TEST_NAME})

  if (BUILD_SHARED_LIBS)
    if (UNIX)
      install(TARGETS ${_NAME}
          LIBRARY
          DESTINATION lib)
    elseif (WIN32)
      install(TARGETS ${_NAME}
          RUNTIME
          DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif ()
  endif ()
  # Generate precompiled header
  if (USE_COREPCH)
    set(_header "${_PCH_HEADER}")
    if (_header STREQUAL "")
    else ()
      add_cxx_pch(${_NAME} ${_PCH_HEADER})
    endif ()
  endif ()
  add_test(NAME ${_NAME} COMMAND ${_NAME})
endfunction(rendu_add_test)

function(rendu_cc_test)
  cmake_parse_arguments(RD_CC_TEST
      ""
      "NAME"
      "SRCS;COPTS;DEFINES;LINKOPTS;DEPS"
      ${ARGN}
      )

  set(_NAME "${PROJECT_NAME}_${RD_CC_TEST_NAME}")

  add_executable(${_NAME} "")
  target_sources(${_NAME} PRIVATE ${RD_CC_TEST_SRCS})

  target_compile_options(${_NAME}
      PRIVATE ${RD_CC_TEST_COPTS}
      )

  target_link_libraries(${_NAME}
      PUBLIC ${RD_CC_TEST_DEPS}
      PRIVATE ${RD_CC_TEST_LINKOPTS}
      )

  add_test(NAME ${_NAME} COMMAND ${_NAME})
endfunction(rendu_cc_test)

function(rendu_batch_test)
  cmake_parse_arguments(RD_BATCH_TEST
      ""
      "NAME"
      "CMAKE_CUR_SOURCE_DIR;CMAKE_CUR_BINARY_DIR;CMAKE_BINARY_DIR;COPTS;DEFINES;LINKOPTS;DEPS"
      ${ARGN}
      )

  CollectSourceFiles(
      ${RD_BATCH_TEST_CMAKE_CUR_SOURCE_DIR}
      PRIVATE_SOURCES
  )

  foreach (file_path ${PRIVATE_SOURCES})
    string(REGEX MATCHALL "[0-9A-Za-z_]*.cpp" tmp ${file_path})
    string(REGEX REPLACE ".cpp" "" filename ${tmp})
    message(STATUS "[test] " ${filename})
    set(_NAME "${filename}")
    add_executable(${_NAME} "")
    target_sources(${_NAME} PRIVATE ${file_path})
    target_compile_options(${_NAME}
        PRIVATE ${RD_BATCH_TEST_COPTS}
        )
    target_link_libraries(${_NAME}
        PUBLIC ${RD_BATCH_TEST_DEPS}
        PRIVATE ${RD_BATCH_TEST_LINKOPTS}
        )
    add_test(NAME ${_NAME} COMMAND ${_NAME})
  endforeach ()
endfunction(rendu_batch_test)
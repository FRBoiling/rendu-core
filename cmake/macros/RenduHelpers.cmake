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
# PROJECT: name of project (see Note)
# NAME: name of target (see Note)
# HDRS: List of public header files for the library
# SRCS: List of source files for the library
# DEPS: List of other libraries to be linked in to the binary targets
# COPTS: List of private compile options
# DEFINES: List of public defines
# LINKOPTS: List of link options
# SETTING: Add settings.cmake interface
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
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;SETTING"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(target_name ${RD_PROJECT}_${RD_NAME})
  set(src_dir ${RENDU_PROJECT_DIR}/${RD_PREFIX})

  message(STATUS "[lib ] " ${target_name})

  CollectAllFiles(
      ${src_dir}
      ${target_name}_srcs
      # Exclude
      ${src_dir}/precompiled_headers
  )

  if (USE_PCH)
    CollectHeaderFiles(
        ${src_dir}/precompiled_headers
        precompiled_headers
    )
  endif (USE_PCH)

  list(APPEND target_srcs
      ${${target_name}_srcs}
      ${precompiled_headers}
      )

  list(APPEND temp_SRCS
      ${target_srcs}
      )
  foreach (src_file IN LISTS temp_SRCS)
    if (${src_file} MATCHES ".*\\.(h|inc)")
      list(REMOVE_ITEM temp_SRCS "${src_file}")
    endif ()
  endforeach ()

  if (temp_SRCS STREQUAL "")
    set(is_interface 1)
  else ()
    set(is_interface 0)
  endif ()

  GroupSources(${src_dir})

  CollectIncludeDirectories(
      ${src_dir}
      include_dirs
      # Exclude
      ${src_dir}/precompiled_headers
  )

  if (is_interface) #TODO:BOIL head only
    add_library(${target_name}
        INTERFACE
        )
    target_sources(${target_name}
        INTERFACE
        ${target_srcs}
        )

    target_link_libraries(${target_name}
        INTERFACE
        ${RD_SETTING}
        ${RD_DEPS}
        )
    target_include_directories(${target_name}
        INTERFACE
        "$<BUILD_INTERFACE:${include_dirs}>"
        )
    target_compile_definitions(${target_name} INTERFACE ${RD_DEFINES})

  else ()
    add_library(${target_name})
    target_sources(${target_name} PRIVATE ${target_srcs})
    target_link_libraries(${target_name}
        PRIVATE
        ${RD_SETTING}
        PUBLIC
        ${RD_DEPS}
        )
    target_include_directories(${target_name}
        PUBLIC
        ${include_dirs}
        )
    target_compile_definitions(${target_name} PUBLIC ${RD_DEFINES})
  endif ()
  #  add_dependencies(${target_name} ${RENDU_LIB_CMAKE_BINARY_DIR}/revision_data.h)
  #  set_target_properties(${target_name}
  #      PROPERTIES
  #      FOLDER
  #      ${RD_PROJECT})

  if (BUILD_SHARED_LIBS)
    message(STATUS "build shared libs")
    if (UNIX)
      install(TARGETS ${target_name}
          LIBRARY
          DESTINATION lib)
    elseif (WIN32)
      install(TARGETS ${target_name}
          RUNTIME
          DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif ()
  endif ()

  # Generate precompiled header
  if (USE_PCH)
    message(STATUS "use precompiled header !")
    set(headers "${precompiled_headers}")
    if (headers STREQUAL "")
    else ()
      add_cxx_pch(${target_name} ${precompiled_headers})
    endif ()
  endif ()
  add_library(${RD_PROJECT}::${RD_NAME} ALIAS ${target_name})
  #  message(STATUS " [ALIAS]" ${RD_PROJECT}::${RD_NAME})
endfunction(rendu_add_library)

function(rendu_add_executable)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;SETTING"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(target_name ${RD_PROJECT}_${RD_NAME})
  set(src_dir ${RENDU_PROJECT_DIR}/${RD_PREFIX})

  message(STATUS "[exec] " ${target_name})

  CollectAllFiles(
      ${src_dir}
      ${target_name}_srcs
      # Exclude
      ${src_dir}/precompiled_headers
  )

  if (USE_PCH)
    CollectHeaderFiles(
        ${src_dir}/precompiled_headers
        precompiled_headers
    )
  endif (USE_PCH)

  list(APPEND target_srcs
      ${${target_name}_srcs}
      ${precompiled_headers}
      )

  GroupSources(${src_dir})


  add_executable(${target_name} ${target_srcs})


  target_link_libraries(${target_name}
      PRIVATE
      ${RD_SETTING}
      PUBLIC
      ${RD_DEPS}
      )

  CollectIncludeDirectories(
      ${src_dir}
      include_dirs
      # Exclude
      ${src_dir}/precompiled_headers)

  target_include_directories(${target_name}
      PRIVATE
      ${include_dirs}
      )

  #  set_target_properties(${target_name}
  #      PROPERTIES
  #      FOLDER
  #      ${RD_EXEC_NAME})


  # Generate precompiled header
  if (USE_PCH)
    message(STATUS "use precompiled header !")
    set(headers "${precompiled_headers}")
    if (headers STREQUAL "")
    else ()
      add_cxx_pch(${target_name} ${precompiled_headers})
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
  set(target_name "${PROJECT_NAME}_${RENDU_TEST_NAME}")
  message(STATUS "[test] " ${target_name})
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

  if (USE_PCH)
    CollectHeaderFiles(
        ${RENDU_TEST_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
        _PCH_HEADER
    )
    #    set(_PCH_HEADER precompiled_headers/${RENDU_TEST_NAME}_pch.h)
  endif (USE_PCH)

  GroupSources(${RENDU_TEST_CMAKE_CUR_SOURCE_DIR})
  add_library(${target_name} "")
  target_sources(${target_name} PRIVATE ${PRIVATE_SOURCES})
  target_link_libraries(${target_name}
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
  target_include_directories(${target_name}
      PUBLIC
      # Provide the binary _dir for all child targets
      ${RENDU_TEST_CMAKE_BINARY_DIR}
      ${PUBLIC_INCLUDES}
      PRIVATE
      ${RENDU_TEST_CMAKE_CUR_BINARY_DIR})
  set_target_properties(${target_name}
      PROPERTIES
      FOLDER
      ${RENDU_TEST_NAME})

  if (BUILD_SHARED_LIBS)
    if (UNIX)
      install(TARGETS ${target_name}
          LIBRARY
          DESTINATION lib)
    elseif (WIN32)
      install(TARGETS ${target_name}
          RUNTIME
          DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif ()
  endif ()
  # Generate precompiled header
  if (USE_PCH)
    set(_header "${_PCH_HEADER}")
    if (_header STREQUAL "")
    else ()
      add_cxx_pch(${target_name} ${_PCH_HEADER})
    endif ()
  endif ()
  add_test(NAME ${target_name} COMMAND ${target_name})
endfunction(rendu_add_test)

function(rendu_cc_test)
  cmake_parse_arguments(RD_CC_TEST
      ""
      "NAME"
      "SRCS;COPTS;DEFINES;LINKOPTS;DEPS"
      ${ARGN}
      )

  set(target_name "${PROJECT_NAME}_${RD_CC_TEST_NAME}")

  add_executable(${target_name} "")
  target_sources(${target_name} PRIVATE ${RD_CC_TEST_SRCS})

  target_compile_options(${target_name}
      PRIVATE ${RD_CC_TEST_COPTS}
      )

  target_link_libraries(${target_name}
      PUBLIC ${RD_CC_TEST_DEPS}
      PRIVATE ${RD_CC_TEST_LINKOPTS}
      )

  add_test(NAME ${target_name} COMMAND ${target_name})
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
    set(target_name "${filename}")
    add_executable(${target_name} "")
    target_sources(${target_name} PRIVATE ${file_path})
    target_compile_options(${target_name}
        PRIVATE ${RD_BATCH_TEST_COPTS}
        )
    target_link_libraries(${target_name}
        PUBLIC ${RD_BATCH_TEST_DEPS}
        PRIVATE ${RD_BATCH_TEST_LINKOPTS}
        )
    add_test(NAME ${target_name} COMMAND ${target_name})
  endforeach ()
endfunction(rendu_batch_test)
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
          file(GLOB COLLECTED_SOURCES
              ${_element_name}/CMakeLists.txt
              )
          set(target_srcs_str "${COLLECTED_SOURCES}")
          if (target_srcs_str STREQUAL "")
            message("can't find CMakeLists.txt in  ${_element_name}!")
          else ()
            add_subdirectory(${_element_name})
            #            message("add_subdirectory ${_element_name}!")
          endif ()
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
      "PROJECT;NAME;SETTING;DIR"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(pub_lib_target "${RD_PROJECT}_${RD_NAME}")
  set(header_lib_target "${pub_lib_target}_obj")

  list(APPEND target_hdrs ${RD_HDRS})
  list(APPEND target_srcs ${RD_SRCS})

  if ("${target_srcs}${target_hdrs}" STREQUAL "")
    CollectSourceFiles(
        ${RD_DIR}
        ${target_name}_srcs
    )
    CollectHeaderFiles(
        ${RD_DIR}
        ${target_name}_hdrs
        # Exclude
        ${RD_DIR}/precompiled_headers
    )
    if (USE_PCH)
      CollectHeaderFiles(
          ${RD_DIR}/precompiled_headers
          precompiled_headers
      )
    endif (USE_PCH)
    list(APPEND target_srcs ${${target_name}_srcs})
    list(APPEND target_hdrs ${${target_name}_hdrs} ${precompiled_headers})
  endif ()

  if ("${target_srcs}" STREQUAL "")
    set(is_interface 1)
  else ()
    set(is_interface 0)
  endif ()

  if ("${target_srcs}${target_hdrs}" STREQUAL "")
    message(STATUS ${target_name} " can't find src files!")
  else ()
    GroupSources(${RD_DIR})
    CollectIncludeDirectories(
        ${RD_DIR}
        include_dirs
        # Exclude
        ${RD_DIR}/precompiled_headers
    )
    if (is_interface) #TODO:BOIL head only
      add_library(${pub_lib_target} INTERFACE)
      target_sources(${pub_lib_target} INTERFACE ${target_hdrs})
      target_include_directories(${pub_lib_target} INTERFACE "$<BUILD_INTERFACE:${include_dirs}>")
      target_compile_definitions(${pub_lib_target} INTERFACE ${RD_DEFINES})
      target_link_libraries(${pub_lib_target} INTERFACE ${RD_SETTING} ${RD_DEPS})
    else ()
      if (BUILD_SHARED_LIBS)
        add_library(${pub_lib_target} SHARED $<TARGET_OBJECTS:${obj_lib_target}>)
      else ()
        add_library(${pub_lib_target} STATIC $<TARGET_OBJECTS:${obj_lib_target}>)
      endif ()
      target_sources(${pub_lib_target} PRIVATE ${target_hdrs} ${target_srcs})
      target_link_libraries(${pub_lib_target} PRIVATE ${RD_SETTING} PUBLIC ${RD_DEPS})
      target_include_directories(${pub_lib_target} PUBLIC ${include_dirs})
      target_compile_definitions(${pub_lib_target} PUBLIC ${RD_DEFINES})
      #  add_dependencies(${target_name} ${RENDU_LIB_CMAKE_BINARY_DIR}/revision_data.h)
    endif ()
    set_target_properties(${pub_lib_target} PROPERTIES FOLDER ${RD_PROJECT})
    add_library(${RD_PROJECT}::${RD_NAME} ALIAS ${pub_lib_target})
    # Generate precompiled header
    if (USE_PCH)
      message(STATUS "use precompiled header !")
      set(headers "${precompiled_headers}")
      if (headers STREQUAL "")
      else ()
        add_cxx_pch(${RD_PROJECT}::${RD_NAME} ${precompiled_headers})
      endif ()
    endif ()
    message(STATUS "[lib ]" ${RD_PROJECT}::${RD_NAME})
  endif ()
endfunction(rendu_add_library)

function(rendu_add_executable)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;SETTING;DIR"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(target_name ${RD_PROJECT}_${RD_NAME})
  set(src_dir ${RD_DIR})

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

  add_executable(${target_name} "")
  target_sources(${target_name} PRIVATE ${target_srcs})
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
      PUBLIC
      ${include_dirs}
      )

  set_target_properties(${target_name}
      PROPERTIES
      FOLDER
      ${PROJECT_NAME})


  # Generate precompiled header
  if (USE_PCH)
    message(STATUS "use precompiled header !")
    set(headers "${precompiled_headers}")
    if (headers STREQUAL "")
    else ()
      add_cxx_pch(${target_name} ${precompiled_headers})
    endif ()
  endif ()
  add_executable(${RD_PROJECT}::${RD_NAME} ALIAS ${target_name})

endfunction(rendu_add_executable)

function(rendu_add_test)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;SETTING;DIR"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(target_name ${RD_PROJECT}_${RD_NAME})
  set(src_dir ${RD_DIR})

  message(STATUS "[test] " ${target_name})

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

  add_executable(${target_name} "")
  target_sources(${target_name} PRIVATE ${target_srcs})
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

  add_test(NAME ${target_name} COMMAND ${target_name})

endfunction(rendu_add_test)

function(rendu_export_and_install_lib alias component)
  add_library(${alias} ALIAS ${component})
  string(TOUPPER "${component}_EXPORT" export_macro_name)
  target_include_directories(${component} INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
      $<INSTALL_INTERFACE:include>)
  generate_export_header(
      ${component}
      EXPORT_MACRO_NAME ${export_macro_name}
      EXPORT_FILE_NAME "caf/detail/${component}_export.hpp")
  set_target_properties(libcaf_${component} PROPERTIES
      EXPORT_NAME ${component}
      SOVERSION ${RD_VERSION}
      VERSION ${RD_LIB_VERSION}
      OUTPUT_NAME caf_${component})
  install(TARGETS ${component}
      EXPORT RDTargets
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${component}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${component}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${component})
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/caf"
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      COMPONENT ${component}
      FILES_MATCHING PATTERN "*.hpp")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/caf/detail/${component}_export.hpp"
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/caf/detail/")
endfunction()

function(rendu_add_component)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;SETTING;DIR"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(pub_lib_target "${RD_PROJECT}_${RD_NAME}")
  set(obj_lib_target "${pub_lib_target}_obj")
  set(header_lib_target "${pub_lib_target}_obj")

  list(APPEND target_hdrs ${RD_HDRS})
  list(APPEND target_srcs ${RD_SRCS})

  if ("${target_srcs}${target_hdrs}" STREQUAL "")
    CollectSourceFiles(
        ${RD_DIR}
        ${target_name}_srcs
    )
    CollectHeaderFiles(
        ${RD_DIR}
        ${target_name}_hdrs
        # Exclude
        ${RD_DIR}/precompiled_headers
    )
    if (USE_PCH)
      CollectHeaderFiles(
          ${RD_DIR}/precompiled_headers
          precompiled_headers
      )
    endif (USE_PCH)
    list(APPEND target_srcs ${${target_name}_srcs})
    list(APPEND target_hdrs ${${target_name}_hdrs} ${precompiled_headers})
  endif ()

  if ("${target_srcs}" STREQUAL "")
    set(is_interface 1)
  else ()
    set(is_interface 0)
  endif ()

  if ("${target_srcs}${target_hdrs}" STREQUAL "")
    message(STATUS ${target_name} " can't find src files!")
  else ()
    GroupSources(${RD_DIR})
    CollectIncludeDirectories(${RD_DIR} include_dirs
        # Exclude
        ${RD_DIR}/precompiled_headers
        )
    if (is_interface) #TODO:BOIL head only
      add_library(${pub_lib_target} INTERFACE)
      target_sources(${pub_lib_target} INTERFACE ${target_hdrs})
      target_include_directories(${pub_lib_target} INTERFACE "$<BUILD_INTERFACE:${include_dirs}>")
      target_compile_definitions(${pub_lib_target} INTERFACE ${RD_DEFINES})
      target_link_libraries(${pub_lib_target} INTERFACE ${RD_SETTING} ${RD_DEPS})
    else ()
      add_library(${obj_lib_target} OBJECT ${target_hdrs} ${target_srcs})
      set_property(TARGET ${obj_lib_target} PROPERTY POSITION_INDEPENDENT_CODE ON)
      target_link_libraries(${obj_lib_target} PRIVATE ${RD_SETTING} PUBLIC ${RD_DEPS})
      target_include_directories(${obj_lib_target} PUBLIC ${include_dirs})
      target_compile_definitions(${obj_lib_target} PUBLIC ${RD_DEFINES})
      if (BUILD_SHARED_LIBS)
        add_library(${pub_lib_target} SHARED $<TARGET_OBJECTS:${obj_lib_target}>)
      else ()
        add_library(${pub_lib_target} STATIC $<TARGET_OBJECTS:${obj_lib_target}>)
      endif ()
      target_link_libraries(${pub_lib_target} PRIVATE ${RD_SETTING} PUBLIC ${RD_DEPS})
      target_include_directories(${pub_lib_target} PUBLIC ${include_dirs})
      #  add_dependencies(${target_name} ${RENDU_LIB_CMAKE_BINARY_DIR}/revision_data.h)
    endif ()
    set_target_properties(${pub_lib_target} PROPERTIES FOLDER ${RD_PROJECT})
    add_library(${RD_PROJECT}::${RD_NAME} ALIAS ${pub_lib_target})
    # Generate precompiled header
    if (USE_PCH)
      message(STATUS "use precompiled header !")
      set(headers "${precompiled_headers}")
      if (headers STREQUAL "")
      else ()
        add_cxx_pch(${RD_PROJECT}::${RD_NAME} ${precompiled_headers})
      endif ()
    endif ()
    message(STATUS "[lib ]" ${RD_PROJECT}::${RD_NAME})
  endif ()
endfunction(rendu_add_component)
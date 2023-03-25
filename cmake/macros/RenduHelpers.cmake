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
  set(target_name ${RD_PROJECT}_${RD_NAME})
  set(src_dir ${RD_DIR})

  message(STATUS "[lib ] " ${target_name})

  list(APPEND target_srcs
      ${RD_HDRS}
      ${RD_SRCS}
      )

  set(target_srcs_str "${target_srcs}")
  if (target_srcs_str STREQUAL "")
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
  endif ()

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

  set(target_srcs_str "${target_srcs}")
  if (target_srcs_str STREQUAL "")
    message(STATUS ${target_name} " can't find src files!")
  else ()
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
    set_target_properties(${target_name}
        PROPERTIES
        FOLDER
        ${RD_PROJECT})

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
  set(target_name ${RD_PROJECT}_${RD_NAME})
  set(src_dir ${RD_DIR})
  set(pub_lib_target "${target_name}")
  set(obj_lib_target "${target_name}_obj")
  message(STATUS "[lib ] " ${target_name})
  set(targets ${pub_lib_target} ${obj_lib_target})

  list(APPEND target_srcs
      ${RD_HDRS}
      ${RD_SRCS}
      )

  set(target_srcs_str "${target_srcs}")
  if (target_srcs_str STREQUAL "")
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
  endif ()

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

  set(target_srcs_str "${target_srcs}")
  if (target_srcs_str STREQUAL "")
    message(STATUS ${target_name} " can't find src files!")
  else ()
    GroupSources(${src_dir})

    CollectIncludeDirectories(
        ${src_dir}
        include_dirs
        # Exclude
        ${src_dir}/precompiled_headers
    )

    if (is_interface) #TODO:BOIL head only
      add_library(${pub_lib_target}
          INTERFACE
          )
      target_sources(${pub_lib_target}
          INTERFACE
          ${target_srcs}
          )

      target_include_directories(${pub_lib_target}
          INTERFACE
          "$<BUILD_INTERFACE:${include_dirs}>"
          )
      target_compile_definitions(${pub_lib_target} INTERFACE ${RD_DEFINES})
      target_link_libraries(${pub_lib_target}
          INTERFACE
          ${RD_SETTING}
          ${RD_DEPS}
          )
    else ()
      add_library(${obj_lib_target} OBJECT ${target_srcs})
      set_property(TARGET ${obj_lib_target} PROPERTY POSITION_INDEPENDENT_CODE ON)
      #      target_sources(${obj_lib_target} PRIVATE ${target_srcs})

      target_link_libraries(${obj_lib_target}
          PRIVATE
          ${RD_SETTING}
          PUBLIC
          ${RD_DEPS}
          )

      target_include_directories(${obj_lib_target}
          PUBLIC
          ${include_dirs}
          )
      target_compile_definitions(${obj_lib_target} PUBLIC ${RD_DEFINES})
      add_library(${pub_lib_target}
          $<TARGET_OBJECTS:${obj_lib_target}>)
      target_link_libraries(${pub_lib_target}
          PRIVATE
          ${RD_SETTING}
          PUBLIC
          ${RD_DEPS}
          )
      target_include_directories(${pub_lib_target}
          PUBLIC
          ${include_dirs}
          )
    endif ()
    #  add_dependencies(${target_name} ${RENDU_LIB_CMAKE_BINARY_DIR}/revision_data.h)
    set_target_properties(${pub_lib_target}
        PROPERTIES
        FOLDER
        ${RD_PROJECT})

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
    set_target_properties(${pub_lib_target}
        PROPERTIES
        FOLDER
        ${RD_PROJECT})

#    foreach (target ${targets})
#      target_compile_definitions(${target} PRIVATE "${pub_lib_target}_EXPORTS")
#      target_include_directories(${target} PRIVATE
#          "${CMAKE_CURRENT_SOURCE_DIR}"
#          "${CMAKE_CURRENT_BINARY_DIR}")
#      if (BUILD_SHARED_LIBS)
#        set_target_properties(${target} PROPERTIES
#            CXX_VISIBILITY_PRESET hidden
#            VISIBILITY_INLINES_HIDDEN ON)
#      endif ()
#    endforeach ()
    # Generate precompiled header
    if (USE_PCH)
      message(STATUS "use precompiled header !")
      set(headers "${precompiled_headers}")
      if (headers STREQUAL "")
      else ()
        add_cxx_pch(${target_name} ${precompiled_headers})
      endif ()
    endif ()
    add_library(${RD_PROJECT}::${RD_NAME} ALIAS ${pub_lib_target})
    message(STATUS " [ALIAS]" ${RD_PROJECT}::${RD_NAME})
  endif ()
endfunction(rendu_add_component)
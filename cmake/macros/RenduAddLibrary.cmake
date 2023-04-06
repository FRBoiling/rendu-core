#**********************************
#  Created by boil on 2023/04/06.
#**********************************

include(CMakeParseArguments)
include(CheckCXXSourceCompiles)

#
# rendu_add_library(
#   NAME
#     awesome
#   HDRS
#     "a.h"
#   SRCS
#     "a.cc"
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

#**********************************
#  Created by boil on 2022/10/19.
#**********************************

# Parameters:
# PROJECT: name of project (see Note)
# NAME: name of target (see Note)
# HDRS: List of public header files for the library
# SRCS: List of source files for the library
# EXCLUDE: List of exclude header/source files for the library
# DEPS: List of other libraries to be linked in to the binary targets
# CONFIG: config name
# LINKOPTS: List of link options
# COPTS: List of private compile options
# DEFINES: List of public defines

function(rendu_add_library)
  cmake_parse_arguments(RD_LIB
      ""
      "PROJECT;NAME;DIR"
      "HDRS;SRCS;EXCLUDE;DEPS;LINKOPTS;DEFINES;COPTS;CONFIG"
      ${ARGN}
  )

  if (NOT "${RD_LIB_CONFIG}" STREQUAL "" AND EXISTS "${RD_LIB_DIR}/${RD_LIB_CONFIG}.h.in.cmake")
    if (EXISTS "${RD_LIB_DIR}/${RD_LIB_CONFIG}.h")
      message("exists ${RD_LIB_DIR}/${RD_LIB_CONFIG}.h ")
    else ()
      configure_file(
          "${RD_LIB_DIR}/${RD_LIB_CONFIG}.h.in.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${RD_LIB_CONFIG}.h"
          @ONLY
      )
    endif ()
  endif ()

  set(RD_LIB_TARGET "${RD_LIB_PROJECT}_${RD_LIB_NAME}")
  set(RD_OBJ_TARGET "${RD_LIB_TARGET}_obj")

  list(APPEND RD_TARGET_HDRS ${RD_LIB_HDRS})
  if ("${RD_TARGET_HDRS}" STREQUAL "")
    rendu_collect_header_files(
        ${RD_LIB_DIR}
        RD_TARGET_HDRS
        # Exclude
        ${RD_LIB_DIR}/precompiled_headers
        ${RD_LIB_EXCLUDE}
    )
    if (RD_USE_PCH)
      rendu_collect_header_files(
          ${RD_LIB_DIR}/precompiled_headers
          RD_PCH_HEADERS
          # Exclude
          ${RD_LIB_EXCLUDE}
      )
    endif (RD_USE_PCH)
  endif ()

  list(APPEND RD_TARGET_SRCS ${RD_LIB_SRCS})
  if ("${RD_TARGET_SRCS}" STREQUAL "")
    rendu_collect_source_files(
        ${RD_LIB_DIR}
        RD_TARGET_SRCS
        ${RD_LIB_EXCLUDE}
    )
  endif ()
  rendu_source_groups(${RD_LIB_DIR})
  rendu_collect_include_directories(${RD_LIB_DIR}
      RD_LIB_INCLUDES
      # Exclude
      ${RD_LIB_DIR}/precompiled_headers
      ${RD_LIB_EXCLUDE}
  )
  if ("${RD_TARGET_SRCS}" STREQUAL "")
    if ("${RD_TARGET_HDRS}" STREQUAL "")
      message(STATUS ${RD_LIB_TARGET} " can't find src files!")
    else ()
      add_library(${RD_LIB_TARGET} INTERFACE)
      target_sources(${RD_LIB_TARGET} INTERFACE ${RD_TARGET_HDRS})
      target_include_directories(${RD_LIB_TARGET} INTERFACE "$<BUILD_INTERFACE:${RD_LIB_INCLUDES}>")
      target_compile_options(${RD_LIB_TARGET} INTERFACE ${RD_LIB_COPTS})
      target_compile_definitions(${RD_LIB_TARGET} INTERFACE ${RD_LIB_DEFINES})
      target_link_libraries(${RD_LIB_TARGET} INTERFACE ${RD_LIB_LINKOPTS} ${RD_LIB_DEPS})
    endif ()
  else ()
    add_library(${RD_OBJ_TARGET} OBJECT ${RD_TARGET_HDRS} ${RD_TARGET_SRCS})
    set_property(TARGET ${RD_OBJ_TARGET} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_include_directories(${RD_OBJ_TARGET} PUBLIC ${RD_LIB_INCLUDES})
    target_compile_options(${RD_OBJ_TARGET} PUBLIC ${RD_LIB_COPTS})
    target_compile_definitions(${RD_OBJ_TARGET} PUBLIC ${RD_LIB_DEFINES})
    target_link_libraries(${RD_OBJ_TARGET} PRIVATE ${RD_LIB_LINKOPTS} PUBLIC ${RD_LIB_DEPS})

    if (RD_BUILD_SHARED_LIBS)
      add_library(${RD_LIB_TARGET} SHARED $<TARGET_OBJECTS:${RD_OBJ_TARGET}>)
    else ()
      add_library(${RD_LIB_TARGET} STATIC $<TARGET_OBJECTS:${RD_OBJ_TARGET}>)
    endif ()
    target_link_libraries(${RD_LIB_TARGET} PRIVATE ${RD_LIB_LINKOPTS} PUBLIC ${RD_LIB_DEPS})
    target_include_directories(${RD_LIB_TARGET} PUBLIC ${RD_LIB_INCLUDES})
  endif ()

  set_target_properties(${RD_LIB_TARGET} PROPERTIES FOLDER ${RD_LIB_PROJECT})
  add_library(${RD_LIB_PROJECT}::${RD_LIB_NAME} ALIAS ${RD_LIB_TARGET})
  # Generate precompiled header
  if (RD_USE_PCH)
    message(STATUS "use precompiled header !")
    if ("${RD_PCH_HEADERS}" STREQUAL "")
    else ()
      add_cxx_pch(${RD_LIB_PROJECT}::${RD_LIB_NAME} ${RD_PCH_HEADERS})
    endif ()
  endif ()
  message(STATUS "[lib ]" ${RD_LIB_PROJECT}::${RD_LIB_NAME})

endfunction(rendu_add_library)
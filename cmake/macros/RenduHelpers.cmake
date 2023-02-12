#**********************************
#  Created by boil on 2022/10/19.
#**********************************

include(CMakeParseArguments)
function(_rendu_target_compile_features_if_available TARGET TYPE FEATURE)
  if (FEATURE IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_compile_features(${TARGET} ${TYPE} ${FEATURE})
  else ()
    message(WARNING "Feature ${FEATURE} is unknown for the CXX compiler")
  endif ()
endfunction()

include(CheckCXXSourceCompiles)

check_cxx_source_compiles(
    [==[
#ifdef _MSC_VER
#  if _MSVC_LANG < 201700L
#    error "The compiler defaults or is configured for C++ < 17"
#  endif
#elif __cplusplus < 201700L
#  error "The compiler defaults or is configured for C++ < 17"
#endif
int main() { return 0; }
]==]
    RENDU_INTERNAL_AT_LEAST_CXX17)

if (RENDU_INTERNAL_AT_LEAST_CXX17)
  set(RENDU_INTERNAL_CXX_STD_FEATURE cxx_std_17)
else ()
  set(RENDU_INTERNAL_CXX_STD_FEATURE cxx_std_14)
endif ()

# 将所有子目录添加。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_add_subdirectory(
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/platform)
#
function(rendu_add_subdirectory current_dir)
  list(FIND ARGN "${current_dir}" IS_EXCLUDED)
  if (IS_EXCLUDED EQUAL -1)
    list(APPEND DIRS ${current_dir})
    file(GLOB SUB_DIRECTORIES ${current_dir}/*)
    foreach (SUB_DIRECTORY ${SUB_DIRECTORIES})
      if (IS_DIRECTORY ${SUB_DIRECTORY})
        list(FIND ARGN "${SUB_DIRECTORY}" IS_EXCLUDED)
        if (IS_EXCLUDED EQUAL -1)
          get_filename_component(element_name ${SUB_DIRECTORY} NAME)
          add_subdirectory(${element_name})
        endif ()
      endif ()
    endforeach ()
  endif ()
endfunction()

# Parameters:
# NAME: name of target (see Note)
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
# rendu_add_library(
#   NAME
#     fantastic_lib
#   SRCS
#     "b.cc"
#   DEPS
#     rendu::awesome # not "awesome" !
#   PUBLIC
# )
#
# rendu_add_library(
#   NAME
#     main_lib
#   ...
#   DEPS
#     rendu::fantastic_lib
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
  cmake_parse_arguments(RENDU_LIB
      ""
      "NAME"
      "CMAKE_CUR_SOURCE_DIR;CMAKE_CUR_BINARY_DIR;CMAKE_BINARY_DIR;COPTS;DEFINES;LINKOPTS;DEPS"
      ${ARGN}
      )
  set(_NAME "${PROJECT_NAME}_${RENDU_LIB_NAME}")

  message(STATUS ${_NAME})
  #  message(STATUS ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR})
  #  message(STATUS ${RENDU_LIB_CMAKE_CUR_BINARY_DIR})
  #  message(STATUS ${RENDU_LIB_CMAKE_BINARY_DIR})

  CollectAllFiles(
      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
      PRIVATE_SOURCES
      # Exclude
      #      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/Debugging
      #      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/Platform
      #      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/PrecompiledHeaders
  )

  list(APPEND PRIVATE_SOURCES
      #      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/Debugging/Errors.cpp
      #      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/Debugging/Errors.h
      )

  set(_SRCS "${PRIVATE_SOURCES}")
  foreach (src_file IN LISTS _SRCS)
    if (${src_file} MATCHES ".*\\.(h|inc)")
      list(REMOVE_ITEM _SRCS "${src_file}")
    endif ()
  endforeach ()

  if (_SRCS STREQUAL "")
    set(LIB_IS_INTERFACE 1)
  else ()
    set(LIB_IS_INTERFACE 0)
  endif ()

  if (USE_COREPCH)
    set(PRIVATE_PCH_HEADER precompiled_headers/${RENDU_LIB_NAME}_pch.h)
  endif (USE_COREPCH)

  GroupSources(${RENDU_LIB_CMAKE_CUR_SOURCE_DIR})

  #  add_definitions(-DTRINITY_API_EXPORT_COMMON)


  add_library(${_NAME} ${PRIVATE_SOURCES})
  #  if (LIB_IS_INTERFACE)
  #    target_sources(${_NAME} INTERFACE ${PRIVATE_SOURCES})
  #    target_include_directories(${_NAME}
  #        INTERFACE
  #        # Provide the binary dir for all child targets
  #        ${RENDU_LIB_CMAKE_BINARY_DIR}
  #        ${PUBLIC_INCLUDES}
  #        ${RENDU_LIB_CMAKE_CUR_BINARY_DIR})
  #
  #    target_link_libraries(${_NAME}
  #        INTERFACE
  #        rendu-core-interface
  #        )
  #
  #    target_compile_definitions(${_NAME} INTERFACE ${RENDU_LIB_DEFINES})
  #  else()
  target_sources(${_NAME} PRIVATE ${PRIVATE_SOURCES})
  CollectIncludeDirectories(
      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
      PUBLIC_INCLUDES
      # Exclude
      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/PrecompiledHeaders)
  target_include_directories(${_NAME}
      PUBLIC
      # Provide the binary dir for all child targets
      ${RENDU_LIB_CMAKE_BINARY_DIR}
      ${PUBLIC_INCLUDES}
      PRIVATE
      ${RENDU_LIB_CMAKE_CUR_BINARY_DIR})

  target_link_libraries(${_NAME}
      PRIVATE
      rendu-core-interface
      PUBLIC
      )

  target_compile_definitions(${_NAME} PUBLIC ${RENDU_LIB_DEFINES})
  #  endif ()
  #  add_dependencies(${_NAME} ${RENDU_LIB_CMAKE_BINARY_DIR}/revision_data.h)
  set_target_properties(${_NAME}
      PROPERTIES
      FOLDER
      ${RENDU_LIB_NAME})

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
    add_cxx_pch(${_NAME} ${PRIVATE_PCH_HEADER})
  endif ()
endfunction()

function(rendu_add_executable)
  cmake_parse_arguments(RENDU_LIB
      ""
      "NAME"
      "CMAKE_CUR_SOURCE_DIR;CMAKE_CUR_BINARY_DIR;CMAKE_BINARY_DIR;COPTS;DEFINES;LINKOPTS;DEPS"
      ${ARGN}
      )
  set(_NAME "${PROJECT_NAME}_${RENDU_LIB_NAME}")
  message(STATUS ${_NAME})
  CollectAllFiles(
      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
      PRIVATE_SOURCES
      # Exclude
      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
  )

  list(APPEND PRIVATE_SOURCES
      #      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/Debugging/Errors.cpp
      #      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/Debugging/Errors.h
      )

  if (USE_COREPCH)
    set(PRIVATE_PCH_HEADER precompiled_headers/${RENDU_LIB_NAME}_pch.h)
  endif (USE_COREPCH)

  GroupSources(${RENDU_LIB_CMAKE_CUR_SOURCE_DIR})
  add_executable(${_NAME} ${PRIVATE_SOURCES})
  target_link_libraries(${_NAME}
      PRIVATE
      rendu-core-interface
      PUBLIC
      )
  CollectIncludeDirectories(
      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
      PUBLIC_INCLUDES
      # Exclude
      ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/PrecompiledHeaders)
  target_include_directories(${_NAME}
      PUBLIC
      # Provide the binary dir for all child targets
      ${RENDU_LIB_CMAKE_BINARY_DIR}
      ${PUBLIC_INCLUDES}
      PRIVATE
      ${RENDU_LIB_CMAKE_CUR_BINARY_DIR})
  set_target_properties(${_NAME}
      PROPERTIES
      FOLDER
      ${RENDU_LIB_NAME})

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
    add_cxx_pch(${_NAME} ${PRIVATE_PCH_HEADER})
  endif ()
endfunction()



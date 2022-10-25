#**********************************
#  Created by boil on 2022/10/19.
#**********************************

include(CMakeParseArguments)
include(RenduConfigureCopts)
include(RenduDll)

# The IDE folder for Abseil that will be used if Abseil is included in a CMake
# project that sets
#    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# For example, Visual Studio supports folders.
if(NOT DEFINED RENDU_IDE_FOLDER)
  set(RENDU_IDE_FOLDER Abseil)
endif()

if(RENDU_USE_SYSTEM_INCLUDES)
  set(RENDU_INTERNAL_INCLUDE_WARNING_GUARD SYSTEM)
else()
  set(RENDU_INTERNAL_INCLUDE_WARNING_GUARD "")
endif()

function(_rendu_target_compile_features_if_available TARGET TYPE FEATURE)
  if(FEATURE IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    target_compile_features(${TARGET} ${TYPE} ${FEATURE})
  else()
    message(WARNING "Feature ${FEATURE} is unknown for the CXX compiler")
  endif()
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

if(RENDU_INTERNAL_AT_LEAST_CXX17)
  set(RENDU_INTERNAL_CXX_STD_FEATURE cxx_std_17)
else()
  set(RENDU_INTERNAL_CXX_STD_FEATURE cxx_std_14)
endif()

# rendu_cc_library()
#
# CMake function to imitate Bazel's cc_library rule.
#
# Parameters:
# NAME: name of target (see Note)
# HDRS: List of public header files for the library
# SRCS: List of source files for the library
# DEPS: List of other libraries to be linked in to the binary targets
# COPTS: List of private compile options
# DEFINES: List of public defines
# LINKOPTS: List of link options
# PUBLIC: Add this so that this library will be exported under rendu::
# Also in IDE, target will appear in Abseil folder while non PUBLIC will be in Abseil/internal.
# TESTONLY: When added, this target will only be built if both
#           BUILD_TESTING=ON and RENDU_BUILD_TESTING=ON.
#
# Note:
# By default, rendu_cc_library will always create a library named rendu_${NAME},
# and alias target rendu::${NAME}.  The rendu:: form should always be used.
# This is to reduce namespace pollution.
#
# rendu_cc_library(
#   NAME
#     awesome
#   HDRS
#     "a.h"
#   SRCS
#     "a.cc"
# )
# rendu_cc_library(
#   NAME
#     fantastic_lib
#   SRCS
#     "b.cc"
#   DEPS
#     rendu::awesome # not "awesome" !
#   PUBLIC
# )
#
# rendu_cc_library(
#   NAME
#     main_lib
#   ...
#   DEPS
#     rendu::fantastic_lib
# )
#
# TODO: Implement "ALWAYSLINK"
function(rendu_cc_library project_name target_name)
  cmake_parse_arguments(RENDU_CC_LIB
    "DISABLE_INSTALL;PUBLIC;TESTONLY"
    "NAME"
    "HDRS;SRCS;COPTS;DEFINES;LINKOPTS;DEPS"
    ${ARGN}
  )

  if(RENDU_CC_LIB_TESTONLY AND
      NOT ((BUILD_TESTING AND RENDU_BUILD_TESTING) OR
        (RENDU_BUILD_TEST_HELPERS AND RENDU_CC_LIB_PUBLIC)))
    return()
  endif()

  if(RENDU_ENABLE_INSTALL)
    set(_NAME "${RENDU_CC_LIB_NAME}")
  else()
    set(_NAME "${project_name}_${RENDU_CC_LIB_NAME}")
  endif()

  # Check if this is a header-only library
  # Note that as of February 2019, many popular OS's (for example, Ubuntu
  # 16.04 LTS) only come with cmake 3.5 by default.  For this reason, we can't
  # use list(FILTER...)
  set(RENDU_CC_SRCS "${RENDU_CC_LIB_SRCS}")
  foreach(src_file IN LISTS RENDU_CC_SRCS)
    if(${src_file} MATCHES ".*\\.(h|inc)")
      list(REMOVE_ITEM RENDU_CC_SRCS "${src_file}")
    endif()
  endforeach()

  if(RENDU_CC_SRCS STREQUAL "")
    set(RENDU_CC_LIB_IS_INTERFACE 1)
  else()
    set(RENDU_CC_LIB_IS_INTERFACE 0)
  endif()

  # Determine this build target's relationship to the DLL. It's one of four things:
  # 1. "dll"     -- This target is part of the DLL
  # 2. "dll_dep" -- This target is not part of the DLL, but depends on the DLL.
  #                 Note that we assume any target not in the DLL depends on the
  #                 DLL. This is not a technical necessity but a convenience
  #                 which happens to be true, because nearly every target is
  #                 part of the DLL.
  # 3. "shared"  -- This is a shared library, perhaps on a non-windows platform
  #                 where DLL doesn't make sense.
  # 4. "static"  -- This target does not depend on the DLL and should be built
  #                 statically.
  if (${RENDU_BUILD_DLL})
    if(RENDU_ENABLE_INSTALL)
      rendu_internal_dll_contains(TARGET ${_NAME} OUTPUT _in_dll)
    else()
      rendu_internal_dll_contains(TARGET ${RENDU_CC_LIB_NAME} OUTPUT _in_dll)
    endif()
    if (${_in_dll})
      # This target should be replaced by the DLL
      set(_build_type "dll")
      set(RENDU_CC_LIB_IS_INTERFACE 1)
    else()
      # Building a DLL, but this target is not part of the DLL
      set(_build_type "dll_dep")
    endif()
  elseif(BUILD_SHARED_LIBS)
    set(_build_type "shared")
  else()
    set(_build_type "static")
  endif()

  # Generate a pkg-config file for every library:
  if((_build_type STREQUAL "static" OR _build_type STREQUAL "shared")
     AND RENDU_ENABLE_INSTALL)
    if(NOT RENDU_CC_LIB_TESTONLY)
      if(${target_name}_VERSION)
        set(PC_VERSION "${${target_name}_VERSION}")
      else()
        set(PC_VERSION "head")
      endif()
      foreach(dep ${RENDU_CC_LIB_DEPS})
        if(${dep} MATCHES "^${project_name}::(.*)")
	  # Join deps with commas.
          if(PC_DEPS)
            set(PC_DEPS "${PC_DEPS},")
          endif()
          set(PC_DEPS "${PC_DEPS} ${target_name}_${CMAKE_MATCH_1} = ${PC_VERSION}")
        endif()
      endforeach()
      foreach(cflag ${RENDU_CC_LIB_COPTS})
        if(${cflag} MATCHES "^(-Wno|/wd)")
          # These flags are needed to suppress warnings that might fire in our headers.
          set(PC_CFLAGS "${PC_CFLAGS} ${cflag}")
        elseif(${cflag} MATCHES "^(-W|/w[1234eo])")
          # Don't impose our warnings on others.
        elseif(${cflag} MATCHES "^-m")
          # Don't impose CPU instruction requirements on others, as
          # the code performs feature detection on runtime.
        else()
          set(PC_CFLAGS "${PC_CFLAGS} ${cflag}")
        endif()
      endforeach()
      string(REPLACE ";" " " PC_LINKOPTS "${RENDU_CC_LIB_LINKOPTS}")
      FILE(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/lib/pkgconfig/${project_name}_${_NAME}.pc" CONTENT "\
prefix=${CMAKE_INSTALL_PREFIX}\n\
exec_prefix=\${prefix}\n\
libdir=${CMAKE_INSTALL_FULL_LIBDIR}\n\
includedir=${CMAKE_INSTALL_FULL_INCLUDEDIR}\n\
\n\
Name: ${project_name}_${_NAME}\n\
Description: Abseil ${_NAME} library\n\
URL: https://abseil.io/\n\
Version: ${PC_VERSION}\n\
Requires:${PC_DEPS}\n\
Libs: -L\${libdir} ${PC_LINKOPTS} $<$<NOT:$<BOOL:${RENDU_CC_LIB_IS_INTERFACE}>>:-l${project_name}_${_NAME}>\n\
Cflags: -I\${includedir}${PC_CFLAGS}\n")
      INSTALL(FILES "${CMAKE_BINARY_DIR}/lib/pkgconfig/${project_name}_${_NAME}.pc"
              DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
    endif()
  endif()

  if(NOT RENDU_CC_LIB_IS_INTERFACE)
    if(_build_type STREQUAL "dll_dep")
      # This target depends on the DLL. When adding dependencies to this target,
      # any depended-on-target which is contained inside the DLL is replaced
      # with a dependency on the DLL.
      add_library(${_NAME} STATIC "")
      target_sources(${_NAME} PRIVATE ${RENDU_CC_LIB_SRCS} ${RENDU_CC_LIB_HDRS})
      rendu_internal_dll_targets(
        DEPS ${RENDU_CC_LIB_DEPS}
        OUTPUT _dll_deps
      )
      message(   ${RENDU_CC_LIB_LINKOPTS} ---->)
      target_link_libraries(${_NAME}
        PUBLIC ${_dll_deps}
        PRIVATE rendu-dependency-interface
          ${RENDU_CC_LIB_LINKOPTS}
          ${RENDU_DEFAULT_LINKOPTS}
      )

      if (RENDU_CC_LIB_TESTONLY)
        set(_gtest_link_define "GTEST_LINKED_AS_SHARED_LIBRARY=1")
      else()
        set(_gtest_link_define)
      endif()

      target_compile_definitions(${_NAME}
        PUBLIC
          RENDU_CONSUME_DLL
          "${_gtest_link_define}"
      )

    elseif(_build_type STREQUAL "static" OR _build_type STREQUAL "shared")
      add_library(${_NAME} "")
      target_sources(${_NAME} PRIVATE ${RENDU_CC_LIB_SRCS} ${RENDU_CC_LIB_HDRS})
      target_link_libraries(${_NAME}
      PUBLIC ${RENDU_CC_LIB_DEPS}
      PRIVATE
        ${RENDU_CC_LIB_LINKOPTS}
        ${RENDU_DEFAULT_LINKOPTS}
      )
    else()
      message(FATAL_ERROR "Invalid build type: ${_build_type}")
    endif()

    # Linker language can be inferred from sources, but in the case of DLLs we
    # don't have any .cc files so it would be ambiguous. We could set it
    # explicitly only in the case of DLLs but, because "CXX" is always the
    # correct linker language for static or for shared libraries, we set it
    # unconditionally.
    set_property(TARGET ${_NAME} PROPERTY LINKER_LANGUAGE "CXX")

    target_include_directories(${_NAME} ${RENDU_INTERNAL_INCLUDE_WARNING_GUARD}
      PUBLIC
        "$<BUILD_INTERFACE:${RENDU_COMMON_INCLUDE_DIRS}>"
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_options(${_NAME}
      PRIVATE ${RENDU_CC_LIB_COPTS})
    target_compile_definitions(${_NAME} PUBLIC ${RENDU_CC_LIB_DEFINES})

    # Add all Abseil targets to a a folder in the IDE for organization.
    if(RENDU_CC_LIB_PUBLIC)
      set_property(TARGET ${_NAME} PROPERTY FOLDER ${RENDU_IDE_FOLDER})
    elseif(RENDU_CC_LIB_TESTONLY)
      set_property(TARGET ${_NAME} PROPERTY FOLDER ${RENDU_IDE_FOLDER}/test)
    else()
      set_property(TARGET ${_NAME} PROPERTY FOLDER ${RENDU_IDE_FOLDER}/internal)
    endif()

    if(RENDU_PROPAGATE_CXX_STD)
      # Abseil libraries require C++14 as the current minimum standard.
      # Top-level application CMake projects should ensure a consistent C++
      # standard for all compiled sources by setting CMAKE_CXX_STANDARD.
      _rendu_target_compile_features_if_available(${_NAME} PUBLIC ${RENDU_INTERNAL_CXX_STD_FEATURE})
    else()
      # Note: This is legacy (before CMake 3.8) behavior. Setting the
      # target-level CXX_STANDARD property to RENDU_CXX_STANDARD (which is
      # initialized by CMAKE_CXX_STANDARD) should have no real effect, since
      # that is the default value anyway.
      #
      # CXX_STANDARD_REQUIRED does guard against the top-level CMake project
      # not having enabled CMAKE_CXX_STANDARD_REQUIRED (which prevents
      # "decaying" to an older standard if the requested one isn't available).
      set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD ${RENDU_CXX_STANDARD})
      set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    endif()

    # When being installed, we lose the rendu_ prefix.  We want to put it back
    # to have properly named lib files.  This is a no-op when we are not being
    # installed.
    if(RENDU_ENABLE_INSTALL)
      set_target_properties(${_NAME} PROPERTIES FOLDER ${project-name}
        OUTPUT_NAME "${project_name}_${_NAME}"
        SOVERSION 0
      )
    endif()
  else()
    # Generating header-only library
    add_library(${_NAME} INTERFACE)
    target_include_directories(${_NAME} ${RENDU_INTERNAL_INCLUDE_WARNING_GUARD}
      INTERFACE
        "$<BUILD_INTERFACE:${RENDU_COMMON_INCLUDE_DIRS}>"
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
      )

    if (_build_type STREQUAL "dll")
        set(RENDU_CC_LIB_DEPS ${name}_dll)
    endif()

    target_link_libraries(${_NAME}
      INTERFACE
        ${RENDU_CC_LIB_DEPS}
        ${RENDU_CC_LIB_LINKOPTS}
        ${RENDU_DEFAULT_LINKOPTS}
    )
    target_compile_definitions(${_NAME} INTERFACE ${RENDU_CC_LIB_DEFINES})

    if(RENDU_PROPAGATE_CXX_STD)
      # Abseil libraries require C++14 as the current minimum standard.
      # Top-level application CMake projects should ensure a consistent C++
      # standard for all compiled sources by setting CMAKE_CXX_STANDARD.
      _rendu_target_compile_features_if_available(${_NAME} INTERFACE ${RENDU_INTERNAL_CXX_STD_FEATURE})

      # (INTERFACE libraries can't have the CXX_STANDARD property set, so there
      # is no legacy behavior else case).
    endif()
  endif()

  # TODO currently we don't install googletest alongside abseil sources, so
  # installed abseil can't be tested.
  if(NOT RENDU_CC_LIB_TESTONLY AND RENDU_ENABLE_INSTALL)
    install(TARGETS ${_NAME} EXPORT ${PROJECT_NAME}Targets
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
  endif()

    add_library(${project_name}::${RENDU_CC_LIB_NAME} ALIAS ${_NAME})
endfunction()

# rendu_cc_test()
#
# CMake function to imitate Bazel's cc_test rule.
#
# Parameters:
# NAME: name of target (see Usage below)
# SRCS: List of source files for the binary
# DEPS: List of other libraries to be linked in to the binary targets
# COPTS: List of private compile options
# DEFINES: List of public defines
# LINKOPTS: List of link options
#
# Note:
# By default, rendu_cc_test will always create a binary named rendu_${NAME}.
# This will also add it to ctest list as rendu_${NAME}.
#
# Usage:
# rendu_cc_library(
#   NAME
#     awesome
#   HDRS
#     "a.h"
#   SRCS
#     "a.cc"
#   PUBLIC
# )
#
# rendu_cc_test(
#   NAME
#     awesome_test
#   SRCS
#     "awesome_test.cc"
#   DEPS
#     rendu::awesome
#     GTest::gmock
#     GTest::gtest_main
# )
function(rendu_cc_test target_name)
#  if(NOT (BUILD_TESTING AND RENDU_BUILD_TESTING))
#    return()
#  endif()
#
#  cmake_parse_arguments(RENDU_CC_TEST
#    ""
#    "NAME"
#    "SRCS;COPTS;DEFINES;LINKOPTS;DEPS"
#    ${ARGN}
#  )
#
#  set(_NAME "${target_name}_${RENDU_CC_TEST_NAME}")
#
#  add_executable(${_NAME} "")
#  target_sources(${_NAME} PRIVATE ${RENDU_CC_TEST_SRCS})
#  target_include_directories(${_NAME}
#    PUBLIC ${RENDU_COMMON_INCLUDE_DIRS}
#    PRIVATE ${GMOCK_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
#  )
#
#  if (${RENDU_BUILD_DLL})
#    target_compile_definitions(${_NAME}
#      PUBLIC
#        ${RENDU_CC_TEST_DEFINES}
#        RENDU_CONSUME_DLL
#        GTEST_LINKED_AS_SHARED_LIBRARY=1
#    )
#
#    # Replace dependencies on targets inside the DLL with abseil_dll itself.
#    rendu_internal_dll_targets(
#      DEPS ${RENDU_CC_TEST_DEPS}
#      OUTPUT RENDU_CC_TEST_DEPS
#    )
#  else()
#    target_compile_definitions(${_NAME}
#      PUBLIC
#        ${RENDU_CC_TEST_DEFINES}
#    )
#  endif()
#  target_compile_options(${_NAME}
#    PRIVATE ${RENDU_CC_TEST_COPTS}
#  )
#
#  target_link_libraries(${_NAME}
#    PUBLIC ${RENDU_CC_TEST_DEPS}
#    PRIVATE ${RENDU_CC_TEST_LINKOPTS}
#  )
#  # Add all Abseil targets to a folder in the IDE for organization.
#  set_property(TARGET ${_NAME} PROPERTY FOLDER ${RENDU_IDE_FOLDER}/test)
#
#  if(RENDU_PROPAGATE_CXX_STD)
#    # Abseil libraries require C++14 as the current minimum standard.
#    # Top-level application CMake projects should ensure a consistent C++
#    # standard for all compiled sources by setting CMAKE_CXX_STANDARD.
#    _rendu_target_compile_features_if_available(${_NAME} PUBLIC ${RENDU_INTERNAL_CXX_STD_FEATURE})
#  else()
#    # Note: This is legacy (before CMake 3.8) behavior. Setting the
#    # target-level CXX_STANDARD property to RENDU_CXX_STANDARD (which is
#    # initialized by CMAKE_CXX_STANDARD) should have no real effect, since
#    # that is the default value anyway.
#    #
#    # CXX_STANDARD_REQUIRED does guard against the top-level CMake project
#    # not having enabled CMAKE_CXX_STANDARD_REQUIRED (which prevents
#    # "decaying" to an older standard if the requested one isn't available).
#    set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD ${RENDU_CXX_STANDARD})
#    set_property(TARGET ${_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
#  endif()
#
#  add_test(NAME ${_NAME} COMMAND ${_NAME})
endfunction()

cmake_minimum_required(VERSION 3.14)
project(rendu-core VERSION 2.0.0 LANGUAGES CXX)
set(rendu_key rendu)
set(deps_key deps)
set(tests_key tests)
set(examples_key examples)
set(tools_key examples)

#|--------------------------------
#| rendu core Options
#|--------------------------------
option(RENDU_FLAG_TEST             "Build and run test"    ON)
option(RENDU_FLAG_EXAMPLES         "Build examples"        ON)
option(RENDU_FLAG_COVERAGE         "GCC/Clang coverage"    OFF)
option(RENDU_FLAG_USE_BUNDLED_DEPS "Use bundled deps (e.g. fmt)" ON)

#|--------------------------------
#| Set default build type
#|--------------------------------
if(UNIX)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
  endif()
endif()

#|--------------------------------
#| Set supported build types
#|--------------------------------
set(RENDU_SUPPORTED_BUILD_TYPES Debug Release)

#|--------------------------------
#| Config compiler
#|--------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SUPPRESS_REGENERATION OFF)
set(CMAKE_CONFIGURATION_TYPES "${RENDU_SUPPORTED_BUILD_TYPES}" CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeBuild")

#|--------------------------------
#| CMake modules
#|--------------------------------
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/macros")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")
include(CheckCXXSourceRuns)
include(CheckIncludeFiles)
include(ConfigureScripts)
include(ConfigureBaseTargets)
include(CheckPlatform)
include(AutoAddSubdirectory)
include(GroupSources)
include(AutoCollect)

#|--------------------------------
#| rendu Framework Variables
#|--------------------------------
set(RENDU_PROJECT_NAME ${PROJECT_NAME})
set(RENDU_PROJECT_VERSION ${PROJECT_VERSION})
set(RENDU_PROJECT_FRAMEWORK_DIR "rendu")

#|--------------------------------
#| rendu Framework Configuration
#|--------------------------------
set(RENDU_PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(RENDU_PROJECT_OUTDIR ${RENDU_PROJECT_DIR}/Bin/Output)
message(STATUS "RenDu Project Dir: ${RENDU_PROJECT_DIR}")

set(RENDU_PROJECT_BINDIR ${RENDU_PROJECT_DIR}/Bin)
if(DEFINED CMAKE_BUILD_TYPE)
  if(CMAKE_BUILD_TYPE IN_LIST RENDU_SUPPORTED_BUILD_TYPES)
    set(RENDU_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
  else()
    message(FATAL_ERROR "Error: unknown build type \"${CMAKE_BUILD_TYPE}\"")
  endif()
endif()

message(STATUS "RenDu Binary Dir: ${RENDU_PROJECT_OUTDIR}")
message(STATUS "Build ${RENDU_PROJECT_NAME} [${RENDU_PROJECT_VERSION}] [C++${CMAKE_CXX_STANDARD}] [${CMAKE_BUILD_TYPE}]")


#|---------------------------------------------
#| rendu Framework Interface target and alias
#|---------------------------------------------
add_library(rendu INTERFACE)
add_library(rendu::rendu ALIAS rendu)

#|--------------------------------
#| rendu Framework Dependencies
#|--------------------------------
target_include_directories(rendu INTERFACE ${RENDU_PROJECT_DIR})
if(RENDU_FLAG_USE_BUNDLED_DEPS)
  target_include_directories(rendu INTERFACE ${RENDU_PROJECT_DIR}/deps)
else()
  find_package(fmt REQUIRED)
  if(RENDU_FLAG_TEST)
    find_package(GTest REQUIRED)
  endif()
  target_link_libraries(rendu INTERFACE fmt::fmt-header-only)
endif()

#|--------------------------------
#| rendu Framework Build
#|--------------------------------
if(WIN32)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(rendu INTERFACE /Zc:preprocessor)
  endif()
elseif(UNIX)
  target_compile_options(rendu INTERFACE -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function
      $<$<CONFIG:Debug>:-O0 -g -ggdb>
      $<$<CONFIG:Release>:-O3>
      )

  if(RENDU_FLAG_COVERAGE)
    target_compile_options(rendu INTERFACE -fprofile-arcs -ftest-coverage -lgcov --coverage)
    message(STATUS "Build rendu with coverage")
  endif()
else()
  message(FATAL_ERROR "Current platform is not supported!")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${RENDU_PROJECT_OUTDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${RENDU_PROJECT_OUTDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${RENDU_PROJECT_OUTDIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${RENDU_PROJECT_OUTDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${RENDU_PROJECT_OUTDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${RENDU_PROJECT_OUTDIR}")

link_directories(${RENDU_PROJECT_OUTDIR})

#|--------------------------------
#| Build with test
#|--------------------------------
if(RENDU_FLAG_TEST)
  message(STATUS "Building tests ...")
  add_subdirectory(tests)
endif()

#|--------------------------------
#| Build with examples
#|--------------------------------
if(RENDU_FLAG_EXAMPLES)
  message(STATUS "Building examples...")
  add_subdirectory(examples)
endif()

#|--------------------------------
#| Build with coverage
#|--------------------------------
if(RENDU_FLAG_COVERAGE)
  find_package(codecov)
endif()

#|--------------------------------
#| rendu-core rendu project
#|--------------------------------
cmake_minimum_required (VERSION 3.21.7)

# add this options before PROJECT keyword
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Set project name (must be done AFTER setting configuration types)
set(RENDU_PROJECT_NAME rendu)
set(RENDU_PROJECT_DIR ${CMAKE_SOURCE_DIR})
set(RENDU_PROJECT_VERSION 2.0.0)
project(
    ${RENDU_PROJECT_NAME}
    DESCRIPTION "Gaming meets modern C++ - a study project"
    HOMEPAGE_URL "https://github.com/FRBoiling/rendu-core"
    LANGUAGES C CXX
)
#
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${RENDU_PROJECT_DIR}/bin/archive)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${RENDU_PROJECT_DIR}/bin/library)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RENDU_PROJECT_DIR}/bin/runtime)

#|--------------------------------
#| Set supported build types
#|--------------------------------
#set(RENDU_SUPPORTED_BUILD_TYPES Debug Release RelWithDebInfo)
# build in Release-mode by default if not explicitly set
if (CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")
  set(CMAKE_DEFAULT_BUILD_TYPE "RelWithDebInfo" CACHE INTERNAL "")
endif ()
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif ()

## Set RPATH-handing (CMake parameters)
#set(CMAKE_SKIP_BUILD_RPATH 0)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH 0)
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)


#|--------------------------------
#| rendu-core Framework Configuration
#|--------------------------------
# set macro-directory

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake/macros")
include(ConfigureScripts)
include(CheckCXXSourceRuns)
include(CheckIncludeFiles)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

include(RenduGroupSources)
include(RenduBaseTargets)
include(RenduCheckPlatform)
include(RenduAutoCollect)
include(RenduAddLibrary)
include(RenduAddExecutable)
include(RenduAddSubdirectory)

# Find revision ID and hash of the sourcetree
include(cmake/genrev.cmake)
# set default buildoptions and print them
include(cmake/options.cmake)
# print out the results before continuing
include(cmake/showoptions.cmake)

find_package(PCHSupport)

set(test_dir ${CMAKE_CURRENT_SOURCE_DIR}/test)
if (BUILD_TESTING)
  include(RenduAddTest)
  include(CTest)
  enable_testing()
  # add test project
  set(test_dir "")
endif ()

rendu_add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Exclude
    ${test_dir}
)

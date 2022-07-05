cmake_minimum_required(VERSION 3.16)

# set the project name and version
project(rendu VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckCXXSourceRuns)
include(CheckIncludeFiles)
include(ConfigureScripts)


include(ConfigureBaseTargets)
include(CheckPlatform)

include(GroupSources)
include(AutoCollect)


# add dependencies
add_subdirectory(deps)
# add core sources
add_subdirectory(src)

add_executable(rendu_core main.cpp)

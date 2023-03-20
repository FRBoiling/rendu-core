#**********************************
#  Created by boil on 2022/10/19.
#**********************************

option(USE_PCH      "Use precompiled headers when compiling servers"              0)
option(WITH_DYNAMIC_LINKING "Enable dynamic library linking."                         0)
IsDynamicLinkingRequired(WITH_DYNAMIC_LINKING_FORCED)
if(WITH_DYNAMIC_LINKING AND WITH_DYNAMIC_LINKING_FORCED)
  set(WITH_DYNAMIC_LINKING_FORCED OFF)
endif()
if(WITH_DYNAMIC_LINKING OR WITH_DYNAMIC_LINKING_FORCED)
  set(BUILD_SHARED_LIBS ON)
else()
  set(BUILD_SHARED_LIBS OFF)
endif()
option(WITH_WARNINGS    "Show all warnings during compile"                            0)
set(WITH_SOURCE_TREE    "hierarchical" CACHE STRING "Build the source tree for IDE's.")
set_property(CACHE WITH_SOURCE_TREE PROPERTY STRINGS no flat hierarchical hierarchical-folders)
option(WITHOUT_GIT      "Disable the GIT testing routines"                            1)
option(BUILD_TESTING    "Build test suite"  1)
option(BUILD_TOOLS    "Build tools"  1)

if (BUILD_TESTING)
  set(RENDU_BUILD_TESTING ON)
  set(RENDU_USE_GOOGLETEST_HEAD   ON)
endif ()
option(RENDU_FLAG_EXAMPLES    "Build examples"  1)

if(WINDOWS)
  option(USE_LD_GOLD    "Use GNU gold linker"                                        0)
endif()

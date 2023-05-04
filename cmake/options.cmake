#**********************************
#  Created by boil on 2022/10/19.
#**********************************

option(RD_BUILD_EXAMPLES   "Build examples"                                         1)
option(RD_BUILD_TESTING    "Build unit tests"                                       0)
option(RD_BUILD_TOOLS      "Build tools"                                            1)
option(RD_BUILD_PROTO      "Build protobuf"                                         1)

option(RD_USE_PCH          "Use precompiled headers when compiling servers"         0)
option(RD_WITHOUT_GIT      "Disable the GIT testing routines"                       0)
option(RD_WITH_WARNINGS    "Show all warnings during compile"                       0)

option(RD_WITH_DYNAMIC_LINKING "Enable dynamic library linking."                    0)
IsDynamicLinkingRequired(RD_WITH_DYNAMIC_LINKING_FORCED)
if(RD_WITH_DYNAMIC_LINKING AND RD_WITH_DYNAMIC_LINKING_FORCED)
  set(RD_WITH_DYNAMIC_LINKING_FORCED OFF)
endif()
if(RD_WITH_DYNAMIC_LINKING OR RD_WITH_DYNAMIC_LINKING_FORCED)
  set(RD_BUILD_SHARED_LIBS ON)
else()
  set(RD_BUILD_SHARED_LIBS OFF)
endif()

set(RD_WITH_SOURCE_TREE "hierarchical" CACHE STRING "Build the source tree for IDE's.")
set_property(CACHE RD_WITH_SOURCE_TREE PROPERTY STRINGS no flat hierarchical hierarchical-folders)

if(WINDOWS)
  option(RD_USE_LD_GOLD    "Use GNU gold linker"                                        0)
endif()

#**********************************
#  Created by boil on 2022/10/19.
#**********************************
include(${CMAKE_SOURCE_DIR}/cmake/platform/common.cmake)
configure_platform_settings("LINUX")

# RD_USE_LD_GOLD 链接器配置（Linux专用）
if(RD_USE_LD_GOLD)
  execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version
      ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
  if("${LD_VERSION}" MATCHES "GNU gold")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold")
    message(STATUS "LINUX: Using GNU gold linker")
  else()
    message(WARNING "LINUX: GNU gold linker isn't available, using the default system linker")
  endif()
endif()

# 编译器配置（Linux专用）
message(STATUS "LINUX: Detected compiler: ${CMAKE_C_COMPILER}")
if(CMAKE_C_COMPILER MATCHES "gcc" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/gcc/settings.cmake)
elseif(CMAKE_C_COMPILER MATCHES "icc")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/icc/settings.cmake)
elseif(CMAKE_C_COMPILER MATCHES "clang" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake)
else()
  target_compile_definitions(rendu-compile-option-interface
      INTERFACE -D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")
endif()

#**********************************
#  Created by boil on 2022/10/19.
#**********************************
include(${CMAKE_SOURCE_DIR}/cmake/platform/common.cmake)
configure_platform_settings("MAC")

# 平台专用配置
if(RD_USE_LD_GOLD)
  execute_process(COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version
      ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
  if("${LD_VERSION}" MATCHES "GNU gold")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold")
    message(STATUS "MAC: Using GNU gold linker")
  else()
    message(WARNING "MAC: GNU gold linker isn't available, using the default system linker")
  endif()
endif()

message(STATUS "MAC: Detected compiler: ${CMAKE_C_COMPILER}")
if(CMAKE_C_COMPILER MATCHES "clang" OR CMAKE_C_COMPILER_ID MATCHES "Clang|AppleClang")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake)
elseif(CMAKE_C_COMPILER MATCHES "gcc")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/gcc/settings.cmake)
endif()

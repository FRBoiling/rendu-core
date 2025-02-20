#**********************************
#  Created by boil on 2022/10/19.
#**********************************

# 包含公共配置
include(${CMAKE_SOURCE_DIR}/cmake/platform/common.cmake)
configure_platform_settings("WIN")

# Windows 专用定义（合并为单个 add_definitions 调用）
add_definitions(
    -D_WIN32_WINNT=0x0A00
    -DNTDDI_VERSION=0x0A000007
    -DWIN32_LEAN_AND_MEAN
    -DNOMINMAX
    -DRD_REQUIRED_WINDOWS_BUILD=18362
)

# 编译器配置（增加 MSVC 版本检查）
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # 要求最低 MSVC 2019 版本
  if(MSVC_VERSION VERSION_LESS 1920)
    message(FATAL_ERROR "MSVC 2019 or newer required")
  endif()
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/msvc/settings.cmake)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake)
elseif(CMAKE_CXX_PLATFORM_ID MATCHES "MinGW")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/mingw/settings.cmake)
endif()

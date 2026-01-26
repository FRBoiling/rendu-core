cmake_minimum_required(VERSION 3.24)


# 打印CMake版本
message(STATUS "CMake version: ${CMAKE_VERSION}")

# 项目基本信息
project(RenduCore
        LANGUAGES C CXX
        VERSION 1.2.3
        DESCRIPTION "RenduCore C++"
        HOMEPAGE_URL "https://github.com/xxx/rendu"
)

# 设置 CMake 宏脚本目录
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/macros")
include(${CMAKE_SOURCE_DIR}/cmake/RenduCore.cmake)  # 统一入口，自动加载所有模块

# 添加依赖与核心源码（排除 cmake /.git  /build 目录）
rendu_add_subdirectories(
        DIR
        ${CMAKE_CURRENT_SOURCE_DIR}
        EXCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/.git
        ${CMAKE_CURRENT_SOURCE_DIR}/.vscode
        ${CMAKE_CURRENT_SOURCE_DIR}/.idea
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/build
        ${CMAKE_CURRENT_SOURCE_DIR}/build-make
        ${CMAKE_CURRENT_SOURCE_DIR}/build-test
        ${CMAKE_CURRENT_SOURCE_DIR}/doc
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts
        ${CMAKE_CURRENT_SOURCE_DIR}/Testing
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-release
)

# 可选：添加统一的 uninstall 目标（跨平台）
rendu_add_uninstall_target()
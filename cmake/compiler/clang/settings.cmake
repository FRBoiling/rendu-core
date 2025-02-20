#**********************************
#  Created by boil on 2025/02/19.
#**********************************

include(${CMAKE_SOURCE_DIR}/cmake/compiler/common.cmake)

# 设置编译器标识
set(COMPILER_PREFIX "Clang")
set(CLANG_EXPECTED_VERSION 11.0.0)

# 基础编译指令
target_compile_definitions(rendu-compile-option-interface
    INTERFACE
    -D_BUILD_DIRECTIVE="$<CONFIG>"
    -DUSE_CLANG_COMPILER
)

# AppleClang特殊版本处理
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  set(CLANG_EXPECTED_VERSION 12.0.5)  # Xcode 12.5+对应LLVM 11
endif()

# 版本检查（调用公共宏）
check_compiler_version(${CLANG_EXPECTED_VERSION}
    ${CMAKE_CXX_COMPILER_VERSION}
    ${COMPILER_PREFIX})

# charconv 64位整型转换bug检测（保持核心逻辑）
# charconv检测前需要包含
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <charconv>
#include <cstdint>

int main() {
    uint64_t n;
    std::from_chars(\"0\", \"0\"+1, n);
    return static_cast<int>(n);
}
" CLANG_HAVE_PROPER_CHARCONV)

if(NOT CLANG_HAVE_PROPER_CHARCONV)
  message(STATUS "Clang: 检测到64位from_chars缺陷，已启用兼容方案")
  target_compile_definitions(rendu-compat-interface
      INTERFACE -DRD_NEED_CHARCONV_WORKAROUND)
endif()

# 警告配置（通过公共函数管理）
set(CURRENT_WARNING_FLAGS
    -W
    -Wall
    -Wextra
    -Wimplicit-fallthrough
    -Winit-self
    -Wfatal-errors
    -Wno-mismatched-tags
    -Woverloaded-virtual
)
configure_warnings()

# 调试配置
if(WITH_COREDEBUG)
  target_compile_options(rendu-debug-interface INTERFACE -g3)
  message(STATUS "Clang: 调试标志已设置 (-g3)")
endif()

# Sanitizer统一配置
set(ASAN_FLAGS
    -fno-omit-frame-pointer
    -fsanitize=address
    -fsanitize-recover=address
    -fsanitize-address-use-after-scope)
configure_sanitizers(ASAN)

set(MSAN_FLAGS
    -fno-omit-frame-pointer
    -fsanitize=memory
    -fsanitize-memory-track-origins
    -mllvm -msan-keep-going=1)
configure_sanitizers(MSAN)

set(UBSAN_FLAGS
    -fno-omit-frame-pointer
    -fsanitize=undefined)
configure_sanitizers(UBSAN)

set(TSAN_FLAGS
    -fno-omit-frame-pointer
    -fsanitize=thread)
configure_sanitizers(TSAN)

# 构建时间分析
if(BUILD_TIME_ANALYSIS)
  target_compile_options(rendu-perf-interface INTERFACE -ftime-trace)
  message(STATUS "Clang: 启用编译耗时分析")
endif()

# 特殊警告抑制
target_compile_options(rendu-suppress-interface
    INTERFACE
    -Wno-narrowing        # 抑制g3d相关警告
    -Wno-deprecated-register  # 屏蔽gsoap警告
    -Wno-undefined-inline)    # fmt优化hack

# 共享库配置
if(BUILD_SHARED_LIBS)
  target_compile_options(rendu-shared-interface
      INTERFACE
      -fPIC
      -fvisibility=hidden)
  target_link_options(rendu-shared-interface
      INTERFACE
      --no-undefined)
  message(STATUS "Clang: 共享库模式配置完成")
endif()

# PCH模板实例化优化（保留核心检测逻辑）
set(CMAKE_REQUIRED_FLAGS "-fpch-instantiate-templates")
check_cxx_source_compiles("int main() { return 0; }" CLANG_HAS_PCH_INSTANTIATE_TEMPLATES)
unset(CMAKE_REQUIRED_FLAGS)

if(CLANG_HAS_PCH_INSTANTIATE_TEMPLATES)
  target_compile_options(rendu-pch-interface
      INTERFACE -fpch-instantiate-templates)
  message(STATUS "Clang: 启用PCH模板预实例化优化")
endif()

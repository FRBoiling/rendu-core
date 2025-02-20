#**********************************
#  Created by boil on 2025/02/19.
#**********************************

include(${CMAKE_SOURCE_DIR}/cmake/compiler/common.cmake)

# 设置编译器标识
set(COMPILER_PREFIX "ICC")
set(ICC_EXPECTED_VERSION 2021.0)

# 基础配置
target_compile_definitions(rendu-compile-option-interface
    INTERFACE
    -D_BUILD_DIRECTIVE="$<CONFIG>"
    -DUSE_INTEL_COMPILER
)

# 版本检查
check_compiler_version(${ICC_EXPECTED_VERSION}
    ${CMAKE_CXX_COMPILER_VERSION}
    ${COMPILER_PREFIX})

# 架构优化配置
if(PLATFORM EQUAL 32)
  target_compile_options(rendu-arch-interface
      INTERFACE
      -axSSE2)
else()
  target_compile_options(rendu-arch-interface
      INTERFACE
      -xSSE2)
endif()
message(STATUS "ICC: SSE2 指令集优化已启用")

# 警告配置
set(CURRENT_WARNING_FLAGS
    -w3         # 启用级别3警告
    -Wcheck
    -Wremarks
)
configure_warnings()

# 调试配置
if(WITH_COREDEBUG)
  target_compile_options(rendu-debug-interface
      INTERFACE
      -g3      # 提升调试信息级别
      -debug extended)
  message(STATUS "ICC: 增强调试模式已启用 (-g3 + extended)")
endif()

# 向量化优化
target_compile_options(rendu-optimize-interface
    INTERFACE
    -qopt-report=3
    -qopt-report-phase=vec)

# 多架构代码生成
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  target_compile_options(rendu-arch-interface
      INTERFACE
      -axCORE-AVX2,SSE4.2)
endif()

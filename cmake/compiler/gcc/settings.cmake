#**********************************
#  Created by boil on 2025/02/19.
#**********************************

include(${CMAKE_SOURCE_DIR}/cmake/compiler/common.cmake)

# 设置编译器标识
set(COMPILER_PREFIX "GCC")
set(GCC_EXPECTED_VERSION 10.0.0)

# 基础配置
target_compile_definitions(rendu-compile-option-interface
    INTERFACE
    -D_BUILD_DIRECTIVE="$<CONFIG>"
    -DUSE_GCC_COMPILER
)

# 版本检查
check_compiler_version(${GCC_EXPECTED_VERSION}
    ${CMAKE_CXX_COMPILER_VERSION}
    ${COMPILER_PREFIX})

# 架构优化
if(PLATFORM EQUAL 32)
  target_compile_options(rendu-arch-interface INTERFACE -msse2 -mfpmath=sse)
endif()

if(RD_SYSTEM_PROCESSOR MATCHES "x86|amd64")
  target_compile_definitions(rendu-arch-interface INTERFACE -DHAVE_SSE2 -D__SSE2__)
endif()

# 警告配置
set(CURRENT_WARNING_FLAGS
    -W
    -Wall
    -Wextra
    -Winit-self
    -Winvalid-pch
    -Wfatal-errors
    -Woverloaded-virtual
    -Wno-missing-field-initializers
)
configure_warnings()

# 消毒剂配置
set(ASAN_FLAGS
    -fno-omit-frame-pointer
    -fsanitize=address
    -fsanitize-recover=address
    -fsanitize-address-use-after-scope)
configure_sanitizers(ASAN)

# 共享库配置
if(BUILD_SHARED_LIBS)
  target_compile_options(rendu-shared-interface INTERFACE -fPIC -Wno-attributes)
  target_compile_options(rendu-hidden-symbols-interface INTERFACE -fvisibility=hidden)
endif()

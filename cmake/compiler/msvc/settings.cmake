#**********************************
#  Created by boil on 2025/02/19.
#**********************************

include(${CMAKE_SOURCE_DIR}/cmake/compiler/common.cmake)

# 设置编译器标识
set(COMPILER_PREFIX "MSVC")
set(MSVC_MIN_REQUIRED 19.32)

# 基础配置
target_compile_definitions(rendu-compile-option-interface
    INTERFACE
    -D_BUILD_DIRECTIVE="$<CONFIG>"
    -DUSE_MSVC_COMPILER
)

# 警告配置
set(CURRENT_WARNING_FLAGS
    /W4
    /WX
    /wd4251
    /wd4275
    /wd4100
    /wd4189
    /wd4456
    /wd4457
    /wd4458
    /wd4459
    /wd4996
    /wd4619
    /wd4512
)
configure_warnings()

# 消毒剂配置
if(ASAN)
  target_compile_options(rendu-sanitizer-interface INTERFACE /fsanitize=address)
  target_compile_definitions(rendu-sanitizer-interface INTERFACE
      -D_DISABLE_STRING_ANNOTATION
      -D_DISABLE_VECTOR_ANNOTATION)
endif()

# 链接优化
macro(DisableIncrementalLinking ...) # 保持原有宏定义
  DisableIncrementalLinking(CMAKE_EXE_LINKER_FLAGS_DEBUG)
  DisableIncrementalLinking(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO)

  # 高级特性
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.30)
    target_compile_options(rendu-modern-interface INTERFACE /std:c++latest /experimental:module)
  endif()

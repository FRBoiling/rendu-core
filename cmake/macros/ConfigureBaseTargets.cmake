# ... 文件头保持不变 ...

# 编译选项接口
add_library(rendu-compile-option-interface INTERFACE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

# 功能特性接口
add_library(rendu-feature-interface INTERFACE)

# 警告级别接口
add_library(rendu-warning-interface INTERFACE)

# 默认接口聚合
add_library(rendu-default-interface INTERFACE)
target_link_libraries(rendu-default-interface
    INTERFACE
    rendu-compile-option-interface
    rendu-feature-interface)

# 警告抑制接口
add_library(rendu-no-warning-interface INTERFACE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(rendu-no-warning-interface INTERFACE /W0)
else()
  target_compile_options(rendu-no-warning-interface INTERFACE -w)
endif()

# 符号隐藏接口
add_library(rendu-hidden-symbols-interface INTERFACE)

# 兼容性接口
add_library(rendu-compat-interface INTERFACE)
target_compile_definitions(rendu-compat-interface INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:__STDC_FORMAT_MACROS>
)

# 警告屏蔽接口
add_library(rendu-suppress-interface INTERFACE)
target_compile_options(rendu-suppress-interface INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wno-deprecated-declarations>
)

# 预编译头接口
add_library(rendu-pch-interface INTERFACE)
target_compile_options(rendu-pch-interface INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/Yu${CMAKE_SOURCE_DIR}/src/common/stdafx.h>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-include ${CMAKE_SOURCE_DIR}/src/common/stdafx.h>
)

# 依赖项接口聚合
add_library(rendu-dependency-interface INTERFACE)
target_link_libraries(rendu-dependency-interface
    INTERFACE
    rendu-default-interface
    rendu-no-warning-interface
    rendu-hidden-symbols-interface
    rendu-compat-interface
    rendu-suppress-interface
)

# 核心接口聚合
add_library(rendu-core-interface INTERFACE)
target_link_libraries(rendu-core-interface
    INTERFACE
    rendu-default-interface
    rendu-warning-interface
    rendu-pch-interface
)

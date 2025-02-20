# 编译器警告统一配置（跨平台）
function(configure_target_warnings target)
  # 公共警告选项
  set(COMMON_WARNINGS
      -Wall
      -Wextra
      -Wshadow
      -Wnon-virtual-dtor
  )

  # 警告升级为错误
  option(RENDU_WARNINGS_AS_ERRORS "将编译器警告视为错误" ON)
  if(RENDU_WARNINGS_AS_ERRORS)
    list(APPEND COMMON_WARNINGS -Werror)
  endif()

  # 编译器特定配置
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    list(APPEND COMMON_WARNINGS
        -Wpedantic
        -Wcast-align
        -Woverloaded-virtual
        $<$<VERSION_GREATER_EQUAL:CMAKE_CXX_COMPILER_VERSION,12>:-Wundef>
    )
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND COMMON_WARNINGS
        /W4
        /permissive-
        /wd4251 # 类需要具有dll接口
    )
  endif()

  # 应用警告配置到目标
  target_compile_options(${target} INTERFACE
      $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:${COMMON_WARNINGS}>
      $<$<CXX_COMPILER_ID:MSVC>:${COMMON_WARNINGS}>
  )
endfunction()

# 链接到全局警告接口
configure_target_warnings(rendu-warning-interface)

# Release模式额外警告
target_compile_options(rendu-warning-interface INTERFACE
    $<$<CONFIG:Release>:
    $<$<CXX_COMPILER_ID:GNU>:-Wsuggest-final-types>
    $<$<CXX_COMPILER_ID:Clang>:-Weverything>
    >
)

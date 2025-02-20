#**********************************
#  Created by boil on 2025/02/19.
#**********************************

# 版本检查宏
macro(check_compiler_version EXPECTED_VERS CURRENT_VERS COMPILER_NAME)
  if(${CURRENT_VERS} VERSION_LESS ${EXPECTED_VERS})
    message(FATAL_ERROR "${COMPILER_NAME}: 最低要求版本 ${EXPECTED_VERS} (当前版本 ${CURRENT_VERS})")
  else()
    message(STATUS "${COMPILER_NAME}: 版本检查通过 (${CURRENT_VERS} ≥ ${EXPECTED_VERS})")
  endif()
endmacro()

# 消毒剂配置
function(configure_sanitizers TYPE)
  if(${TYPE})
    target_compile_options(rendu-sanitizer-interface INTERFACE ${${TYPE}_FLAGS})
    target_link_options(rendu-sanitizer-interface INTERFACE ${${TYPE}_FLAGS})
    message(STATUS "${CMAKE_CXX_COMPILER_ID}: 已启用 ${TYPE}")
  endif()
endfunction()

# 警告配置基类
function(configure_warnings)
  if(WITH_WARNINGS)
    target_compile_options(rendu-warning-interface INTERFACE ${CURRENT_WARNING_FLAGS})
    message(STATUS "${CMAKE_CXX_COMPILER_ID}: 严格警告模式已启用")
  endif()
endfunction()

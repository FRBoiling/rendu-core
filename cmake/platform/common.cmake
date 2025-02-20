#**********************************
#  Created by boil on 2025/02/19.
#**********************************

#[[
  公共配置模板
  @param PLATFORM_NAME 平台名称 (WIN/MAC/LINUX)
]]
function(configure_platform_settings PLATFORM_NAME)
  # 配置目录设置
  if(NOT CONF_DIR)
    set(CONF_DIR ${CMAKE_INSTALL_PREFIX}/etc CACHE PATH "Configuration directory")
    message(STATUS "${PLATFORM_NAME}: Using default configuration directory")
  endif()

  # 统一配置卸载目标
  configure_file(
      "${CMAKE_SOURCE_DIR}/cmake/platform/cmake_uninstall.in.cmake"
      "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
      @ONLY
  )
  add_custom_target(uninstall
      "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
  )
  message(STATUS "${PLATFORM_NAME}: Configured uninstall target")
endfunction()

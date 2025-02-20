# ... 文件头注释保持不变 ...

# 构建路径空格检查（Windows平台专用）
function(WarnAboutSpacesInBuildPath)
  if(WIN32 AND " ${CMAKE_BINARY_DIR}" MATCHES " ")
    message(WARNING
        "构建路径包含空格可能引发问题:\n"
        "当前路径: ${CMAKE_BINARY_DIR}\n"
        "建议使用不含空格的路径"
    )
  endif()
endfunction()

# 获取脚本基础路径（带有效性验证）
function(GetScriptsBasePath out_var)
  cmake_path(SET script_path NORMALIZE "${CMAKE_SOURCE_DIR}/scripts")
  if(NOT EXISTS "${script_path}")  # 添加双引号处理空格路径
    message(FATAL_ERROR "关键脚本目录不存在: ${script_path}\n"
        "请确认：\n"
        "1. 项目是否完整克隆\n"
        "2. 目录结构是否正确\n"
        "3. 存在有效的脚本模块")
  endif()
  set(${out_var} "${script_path}" PARENT_SCOPE)  # 添加双引号
endfunction()


# 获取指定模块路径（使用现代路径处理）
function(GetPathToScriptModule module out_var)
  GetScriptsBasePath(base_path)
  cmake_path(SET module_path NORMALIZE "${base_path}/${module}")
  set(${out_var} ${module_path} PARENT_SCOPE)
endfunction()

# 生成模块项目名称（修正变量引用）
function(GetProjectNameOfScriptModule module out_var)
  string(TOLOWER "scripts_${module}" generated_name)  # 修正为使用参数module
  set(${out_var} ${generated_name} PARENT_SCOPE)
endfunction()

function(GetScriptModuleList out_var)
  GetScriptsBasePath(base_path)

  # 仅获取顶层目录
  file(GLOB children RELATIVE ${base_path} ${base_path}/*)

  # 严格验证模块目录
  set(valid_modules)
  foreach(child IN LISTS children)
    # 过滤非目录项和隐藏目录
    if(IS_DIRECTORY "${base_path}/${child}" AND NOT child MATCHES "^\\.|_")
      # 验证模块名格式
      if(child MATCHES "^[A-Za-z][A-Za-z0-9_]*$")
        # 检查必要构建文件
        if(EXISTS "${base_path}/${child}/CMakeLists.txt")
          list(APPEND valid_modules ${child})
        else()
          message(WARNING "脚本模块 ${child} 缺少CMakeLists.txt文件")
        endif()
      endif()
    endif()
  endforeach()

  # 空目录处理策略
  if(NOT valid_modules)
    set(msg_type WARNING)  # 默认警告级别
    if(DEFINED RENDU_SCRIPTS_REQUIRED)
      if(RENDU_SCRIPTS_REQUIRED)
        set(msg_type FATAL_ERROR)
      endif()
    else()
      option(RENDU_SCRIPTS_REQUIRED "是否强制需要脚本模块" OFF)  # 默认改为OFF
    endif()

    message(${msg_type} "未找到有效脚本模块\n"
        "扫描路径: ${base_path}\n"
        "可能原因:\n"
        "1. 尚未添加任何脚本模块\n"
        "2. 模块目录名称不符合规范\n"
        "3. 缺少CMakeLists.txt文件")
  endif()

  set(${out_var} ${valid_modules} PARENT_SCOPE)
endfunction()


# 转换模块名为配置变量名
function(ScriptModuleNameToVariable module out_var)
  string(TOUPPER "${module}" upper_module)
  set(${out_var} "SCRIPTS_${upper_module}" PARENT_SCOPE)
endfunction()

# 动态链接需求检查（简化逻辑）
function(IsDynamicLinkingRequired out_var)
  string(TOLOWER "${SCRIPTS}" scripts_config)
  set(is_required OFF)

  if(scripts_config MATCHES "dynamic")
    set(is_required ON)
  else()
    GetScriptModuleList(modules)
    foreach(module IN LISTS modules)
      ScriptModuleNameToVariable(${module} var_name)
      if(${${var_name}} STREQUAL "DYNAMIC")
        set(is_required ON)
        break()
      endif()
    endforeach()
  endif()

  set(${out_var} ${is_required} PARENT_SCOPE)
endfunction()

# 生成平台原生库名（使用CMake内置变量）
function(GetNativeSharedLibraryName module out_var)
  set(lib_name
      "${CMAKE_SHARED_LIBRARY_PREFIX}${module}${CMAKE_SHARED_LIBRARY_SUFFIX}"
  )
  set(${out_var} ${lib_name} PARENT_SCOPE)
endfunction()

# 安装路径生成（支持多配置）
function(GetInstallOffset out_var)
  cmake_path(SET install_dir
      NORMALIZE
      "$<IF:$<CONFIG:Debug>,scripts_debug,scripts>"
  )
  set(${out_var}
      "${CMAKE_INSTALL_PREFIX}/${install_dir}"
      PARENT_SCOPE
  )
endfunction()

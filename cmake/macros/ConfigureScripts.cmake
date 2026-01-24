# ====================================================================
# 模块: ConfigureScripts
# 描述: 脚本模块相关工具函数,提供脚本路径管理、模块配置等功能
# 依赖模块:
#   - 无
# ====================================================================

# ====================================================================
# 函数: rendu_warn_about_spaces_in_build_path
# 描述: 检查构建路径中是否包含空格（仅 Windows 需关注）
#
# 用法示例:
#   rendu_warn_about_spaces_in_build_path()
# ====================================================================
function(rendu_warn_about_spaces_in_build_path)
    if (WIN32)
        string(FIND "${CMAKE_BINARY_DIR}" " " space_index_pos)
        if (space_index_pos GREATER -1)
            message("")
            message(WARNING " *** WARNING!\n"
                    " *** Your selected build directory contains spaces!\n"
                    " *** Please note that this will cause issues!")
        endif ()
    endif ()
endfunction()

# ====================================================================
# 函数: rendu_get_scripts_base_path
# 描述: 获取脚本目录的基础路径
#
# 参数:
#   variable - 输出变量名，用于存储脚本目录路径
#
# 用法示例:
#   rendu_get_scripts_base_path(scripts_dir)
#   message("Scripts directory: ${scripts_dir}")
# ====================================================================
function(rendu_get_scripts_base_path variable)
    set(${variable} "${CMAKE_SOURCE_DIR}/src/server/scripts" PARENT_SCOPE)
endfunction()

# ====================================================================
# 函数: rendu_get_path_to_script_module
# 描述: 获取指定模块的绝对路径
#
# 参数:
#   module   - 模块名称
#   variable - 输出变量名，用于存储模块路径
#
# 用法示例:
#   rendu_get_path_to_script_module("mymodule" module_path)
#   message("Module path: ${module_path}")
# ====================================================================
function(rendu_get_path_to_script_module module variable)
    rendu_get_scripts_base_path(scripts_base_path)
    set(${variable} "${scripts_base_path}/${module}" PARENT_SCOPE)
endfunction()

# ====================================================================
# 函数: rendu_get_project_name_of_script_module
# 描述: 获取指定模块的项目名
#
# 参数:
#   module   - 模块名称
#   variable - 输出变量名，用于存储项目名
#
# 用法示例:
#   rendu_get_project_name_of_script_module("mymodule" project_name)
#   message("Project name: ${project_name}")
# ====================================================================
function(rendu_get_project_name_of_script_module module variable)
    string(TOLOWER "scripts_${module}" generated_name)
    set(${variable} "${generated_name}" PARENT_SCOPE)
endfunction()

# ====================================================================
# 函数: rendu_get_script_module_list
# 描述: 获取所有脚本模块列表
#
# 参数:
#   variable - 输出变量名，用于存储模块列表
#
# 用法示例:
#   rendu_get_script_module_list(module_list)
#   foreach(module IN LISTS module_list)
#       message("Found module: ${module}")
#   endforeach()
# ====================================================================
function(rendu_get_script_module_list variable)
    rendu_get_scripts_base_path(base_path)
    file(GLOB locale_script_module_list RELATIVE
            ${base_path}
            ${base_path}/*)

    set(${variable} "")
    foreach (script_module IN LISTS locale_script_module_list)
        rendu_get_path_to_script_module(${script_module} module_path)
        if (IS_DIRECTORY "${module_path}")
            list(APPEND ${variable} ${script_module})
        endif ()
    endforeach ()
    set(${variable} ${${variable}} PARENT_SCOPE)
endfunction()

# ====================================================================
# 函数: rendu_script_module_name_to_variable
# 描述: 将脚本模块名转换为其对应的变量名
#
# 参数:
#   module   - 模块名称
#   variable - 输出变量名，用于存储转换后的变量名
#
# 用法示例:
#   rendu_script_module_name_to_variable("mymodule" var_name)
#   message("Variable name: ${var_name}")
# ====================================================================
function(rendu_script_module_name_to_variable module variable)
    string(TOUPPER "${module}" upper_module)
    set(var_name "RENDU_SCRIPTS_${upper_module}")
    set(${variable} ${var_name} PARENT_SCOPE)
endfunction()

# ====================================================================
# 函数: rendu_is_dynamic_linking_required
# 描述: 判断是否需要动态链接
#
# 参数:
#   variable - 输出变量名，用于存储是否需要动态链接 (TRUE/FALSE)
#
# 用法示例:
#   rendu_is_dynamic_linking_required(is_required)
#   if (is_required)
#       message("Dynamic linking is required")
#   endif ()
# ====================================================================
function(rendu_is_dynamic_linking_required variable)
    # 检查全局脚本设置是否为动态
    if (RENDU_SCRIPTS MATCHES "dynamic")
        set(is_default_value_dynamic ON)
    else ()
        set(is_default_value_dynamic OFF)
    endif ()

    rendu_get_script_module_list(script_module_list)
    set(is_required OFF)

    foreach (script_module IN LISTS script_module_list)
        rendu_script_module_name_to_variable(${script_module} module_variable)

        if ((NOT DEFINED ${module_variable}) OR
            ${module_variable} STREQUAL "dynamic" OR
            (${module_variable} STREQUAL "default" AND is_default_value_dynamic))
            set(is_required ON)
            break()
        endif ()
    endforeach ()

    set(${variable} ${is_required} PARENT_SCOPE)
endfunction()

# ====================================================================
# 函数: rendu_get_native_shared_library_name
# 描述: 获取平台原生共享库文件名
#
# 参数:
#   module   - 模块名称
#   variable - 输出变量名，用于存储平台特定的库文件名
#
# 用法示例:
#   rendu_get_native_shared_library_name("mymodule" lib_name)
#   message("Library name: ${lib_name}")
# ====================================================================
function(rendu_get_native_shared_library_name module variable)
    if (WIN32)
        set(${variable} "${module}.dll" PARENT_SCOPE)
    elseif (APPLE)
        set(${variable} "lib${module}.dylib" PARENT_SCOPE)
    else ()
        set(${variable} "lib${module}.so" PARENT_SCOPE)
    endif ()
endfunction()

# ====================================================================
# 函数: rendu_get_install_offset
# 描述: 获取脚本安装路径
#
# 参数:
#   variable - 输出变量名，用于存储安装路径
#
# 用法示例:
#   rendu_get_install_offset(install_path)
#   message("Install path: ${install_path}")
# ====================================================================
function(rendu_get_install_offset variable)
    if (WIN32)
        set(${variable} "${CMAKE_INSTALL_PREFIX}/scripts" PARENT_SCOPE)
    else ()
        set(${variable} "${CMAKE_INSTALL_PREFIX}/bin/scripts" PARENT_SCOPE)
    endif ()
endfunction()
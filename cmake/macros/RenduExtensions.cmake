# ====================================================================
# 模块: RenduExtensions
# 描述: 安装相关功能
# 依赖模块:
#   - RenduLogging (日志)
#
# 建议通过 RenduCore.cmake 统一加载所有模块
# ====================================================================

# ====================================================================
# 函数: rendu_install_targets
# 描述: 批量安装目标，支持指定安装类型、目录、组件等
#
# 参数:
#   TARGETS    - 目标列表 (必填)
#   RUNTIME    - 可执行文件安装目录 (可选)
#   LIBRARY    - 动态库安装目录 (可选)
#   ARCHIVE    - 静态库安装目录 (可选)
#   INCLUDES   - 头文件安装目录 (可选)
#   COMPONENT  - 安装组件名 (可选)
#
# 用法示例:
#   rendu_install_targets(
#       TARGETS  mylib myexe
#       RUNTIME  bin
#       LIBRARY  lib
#       ARCHIVE  lib
#       COMPONENT core
#   )
# ====================================================================
function(rendu_install_targets)
    set(options "")
    set(oneValueArgs RUNTIME LIBRARY ARCHIVE INCLUDES COMPONENT)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 检查必需参数
    if (NOT ARG_TARGETS)
        rendu_log_fatal("rendu_install_targets: 必须指定 TARGETS")
    endif ()

    foreach (target IN LISTS ARG_TARGETS)
        install(TARGETS ${target}
            RUNTIME DESTINATION ${ARG_RUNTIME}
            LIBRARY DESTINATION ${ARG_LIBRARY}
            ARCHIVE DESTINATION ${ARG_ARCHIVE}
            INCLUDES DESTINATION ${ARG_INCLUDES}
            COMPONENT ${ARG_COMPONENT}
        )
    endforeach ()

    rendu_log_info("安装目标: ${ARG_TARGETS}")
endfunction()

# ====================================================================
# 函数: rendu_add_uninstall_target
# 描述: 添加标准卸载目标，自动生成 uninstall 脚本
#
# 用法示例:
#   rendu_add_uninstall_target()
# ====================================================================
function(rendu_add_uninstall_target)
    if (NOT TARGET uninstall)
        # 检查卸载脚本模板是否存在
        if (EXISTS "${CMAKE_SOURCE_DIR}/cmake/platform/cmake_uninstall.in.cmake")
            configure_file(
                "${CMAKE_SOURCE_DIR}/cmake/platform/cmake_uninstall.in.cmake"
                "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
                @ONLY
            )
            add_custom_target(uninstall
                COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
                COMMENT "卸载所有已安装文件"
            )
            rendu_log_info("添加卸载目标")
        else ()
            rendu_log_warn("未找到卸载脚本模板，跳过添加卸载目标")
        endif ()
    endif ()
endfunction()

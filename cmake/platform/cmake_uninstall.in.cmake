# 基于 CMake Wiki 修改的卸载脚本
# 注意：需要 CMake 3.5+ 版本支持

# 检查安装清单是否存在
if(NOT EXISTS "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: \"@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt\"")
endif()

# 读取并解析安装清单
file(READ "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")

# 遍历删除文件
foreach(file ${files})
  message(STATUS "Uninstalling \"${file}\"")

  if(EXISTS "${file}")
    # 使用 CMake 文件删除命令
    execute_process(
        COMMAND "@CMAKE_COMMAND@" -E remove "${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )

    # 错误处理
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Failed to remove \"${file}\"")
    endif()
  else()
    message(STATUS "File \"${file}\" does not exist")
  endif()
endforeach()

# 可选：删除清单文件本身（取消注释启用）
# execute_process(
#   COMMAND "@CMAKE_COMMAND@" -E remove "@CMAKE_CURRENT_BINARY_DIR@/install_manifest.txt"
# )

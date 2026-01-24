# ====================================================================
# 模块: EnsureVersion
# 描述: 版本号检测与比较工具宏,提供版本号标准化、比较和范围检查
# 依赖模块:
#   - 无
# ====================================================================
#
# 用法示例:
# rendu_ensure_version("2.5.31" "flex 2.5.4a" version_ok)
# rendu_ensure_version_range("0.1.0" ${foocode_version} "0.7.0" foo_version_ok)
#

# ====================================================================
# 版本号标准化宏
# 将版本号字符串转换为可比较的数值格式
# ====================================================================
macro(rendu_normalize_version _requested_version _normalized_version)
    string(REGEX MATCH "[^0-9]*[0-9]+\\.[0-9]+\\.[0-9]+.*" _threePartMatch "${_requested_version}")
    if (_threePartMatch)
        string(REGEX REPLACE "[^0-9]*([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" _major_vers "${_requested_version}")
        string(REGEX REPLACE "[^0-9]*[0-9]+\\.([0-9]+)\\.[0-9]+.*" "\\1" _minor_vers "${_requested_version}")
        string(REGEX REPLACE "[^0-9]*[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" _patch_vers "${_requested_version}")
    else ()
        string(REGEX REPLACE "([0-9]+)\\.[0-9]+" "\\1" _major_vers "${_requested_version}")
        string(REGEX REPLACE "[0-9]+\\.([0-9]+)" "\\1" _minor_vers "${_requested_version}")
        set(_patch_vers "0")
    endif ()
    math(EXPR ${_normalized_version} "${_major_vers}*10000 + ${_minor_vers}*100 + ${_patch_vers}")
endmacro()

# ====================================================================
# 宏: rendu_check_range_inclusive_lower
# 描述: 检查版本是否在指定范围内（包含下限，不包含上限）
#
# 参数:
#   _lower_limit - 下限值
#   _value       - 待检查的值
#   _upper_limit - 上限值
#   _ok          - 结果变量名 (TRUE/FALSE)
# ====================================================================
macro(rendu_check_range_inclusive_lower _lower_limit _value _upper_limit _ok)
    if (${_value} LESS ${_lower_limit})
        set(${_ok} FALSE)
    elseif (${_value} EQUAL ${_lower_limit})
        set(${_ok} TRUE)
    elseif (${_value} EQUAL ${_upper_limit})
        set(${_ok} FALSE)
    elseif (${_value} GREATER ${_upper_limit})
        set(${_ok} FALSE)
    else ()
        set(${_ok} TRUE)
    endif ()
endmacro()

# ====================================================================
# 宏: rendu_ensure_version
# 描述: 检查 found_version 是否满足 requested_version 的最低要求
#
# 参数:
#   requested_version - 要求的最低版本
#   found_version     - 实际找到的版本
#   var_ok           - 结果变量名 (TRUE/FALSE)
# ====================================================================
macro(rendu_ensure_version requested_version found_version var_ok)
    rendu_normalize_version(${requested_version} req_vers_num)
    rendu_normalize_version(${found_version} found_vers_num)
    if (found_vers_num LESS req_vers_num)
        set(${var_ok} FALSE)
    else ()
        set(${var_ok} TRUE)
    endif ()
endmacro()

# ====================================================================
# 宏: rendu_ensure_version2
# 描述: rendu_ensure_version 的别名实现，保持向后兼容
#
# 参数:
#   requested_version2 - 要求的最低版本
#   found_version2     - 实际找到的版本
#   var_ok2            - 结果变量名 (TRUE/FALSE)
# ====================================================================
macro(rendu_ensure_version2 requested_version2 found_version2 var_ok2)
    rendu_ensure_version(${requested_version2} ${found_version2} ${var_ok2})
endmacro()

# ====================================================================
# 宏: rendu_ensure_version_range
# 描述: 检查 found_version 是否在 min_version 和 max_version 之间
#
# 参数:
#   min_version  - 最低版本
#   found_version - 实际找到的版本
#   max_version  - 最高版本
#   var_ok       - 结果变量名 (TRUE/FALSE)
# ====================================================================
macro(rendu_ensure_version_range min_version found_version max_version var_ok)
    rendu_normalize_version(${min_version} req_vers_num)
    rendu_normalize_version(${found_version} found_vers_num)
    rendu_normalize_version(${max_version} max_vers_num)
    rendu_check_range_inclusive_lower(${req_vers_num} ${found_vers_num} ${max_vers_num} ${var_ok})
endmacro()
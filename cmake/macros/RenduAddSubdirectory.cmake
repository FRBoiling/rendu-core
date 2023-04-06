#**********************************
#  Created by boil on 2023/04/06.
#**********************************

#include(CMakeParseArguments)
#include(CheckCXXSourceCompiles)

# 将所有子目录添加。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_add_subdirectory(
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/platform)
#

function(rendu_add_subdirectory _cur_dir)
  list(FIND ARGN "${_cur_dir}" _is_excluded)
  if (_is_excluded EQUAL -1)
    list(APPEND _dir ${_cur_dir})
    file(GLOB _sub_dirs ${_cur_dir}/*)
    foreach (_sub_dir ${_sub_dirs})
      if (IS_DIRECTORY ${_sub_dir})
        list(FIND ARGN "${_sub_dir}" _is_excluded)
        if (_is_excluded EQUAL -1)
          get_filename_component(_element_name ${_sub_dir} NAME)
          file(GLOB COLLECTED_SOURCES
              ${_element_name}/CMakeLists.txt
              )
          set(target_srcs_str "${COLLECTED_SOURCES}")
          if (target_srcs_str STREQUAL "")
            message("can't find CMakeLists.txt in  ${_element_name}!")
          else ()
            add_subdirectory(${_element_name})
            #            message("add_subdirectory ${_element_name}!")
          endif ()
        endif ()
      endif ()
    endforeach ()
  endif ()
endfunction(rendu_add_subdirectory)
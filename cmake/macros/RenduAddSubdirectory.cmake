#**********************************
#  Created by boil on 2023/04/06.
#**********************************

# 将所有子目录添加。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_add_subdirectory(
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/precompiled_headers
#   ${RENDU_LIB_CMAKE_CUR_SOURCE_DIR}/platform)
#

function(rendu_add_subdirectory current_dir)
  list(FIND ARGN "${current_dir}" IS_EXCLUDED)
  if (IS_EXCLUDED EQUAL -1)
    file(GLOB sub_dirs ${current_dir}/*)
    foreach (sub_dir ${sub_dirs})
      if (IS_DIRECTORY ${sub_dir})
        list(FIND ARGN "${sub_dir}" IS_EXCLUDED)
        if (IS_EXCLUDED EQUAL -1)
          get_filename_component(element_name ${sub_dir} NAME)
          file(GLOB COLLECTED_SOURCES
              ${element_name}/CMakeLists.txt
              )
          set(target_srcs_str "${COLLECTED_SOURCES}")
          if (target_srcs_str STREQUAL "")
            message("can't find CMakeLists.txt in  ${element_name}!")
          else ()
            add_subdirectory(${element_name})
            #            message("add_subdirectory ${element_name}!")
          endif ()
        endif ()
      endif ()
    endforeach ()
  endif ()
endfunction(rendu_add_subdirectory)
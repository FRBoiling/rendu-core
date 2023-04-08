#**********************************
#  Created by boil on 2022/8/14.
#**********************************
# 将所有源文件收集到给定变量中，用于包含子目录中的所有源。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_collect_source_files(
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers
#   ${CMAKE_CURRENT_SOURCE_DIR}/platform)
#
function(rendu_collect_all_files current_dir variable)
  list(FIND ARGN ${current_dir} IS_EXCLUDED)
  if (IS_EXCLUDED EQUAL -1)
    file(GLOB COLLECTED_FILES
        ${current_dir}/*.c
        ${current_dir}/*.cc
        ${current_dir}/*.cpp
        ${current_dir}/*.pb.cc
        ${current_dir}/*.h
        ${current_dir}/*.hh
        ${current_dir}/*.hpp
        ${current_dir}/*.ipp
        ${current_dir}/*.pb.h
        ${current_dir}/*.inc
        )
    list(APPEND ${variable} ${COLLECTED_FILES})
    file(GLOB SUB_DIRECTORIES ${current_dir}/*)
    foreach (SUB_DIRECTORY ${SUB_DIRECTORIES})
      if (IS_DIRECTORY ${SUB_DIRECTORY})
        rendu_collect_all_files(${SUB_DIRECTORY} ${variable} ${ARGN})
      endif ()
    endforeach ()
    set(${variable} ${${variable}} PARENT_SCOPE)
  endif ()
endfunction(rendu_collect_all_files)


# 将所有源文件收集到给定变量中，用于包含子目录中的所有源。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_collect_source_files(
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers
#   ${CMAKE_CURRENT_SOURCE_DIR}/platform)
#
function(rendu_collect_source_files current_dir variable)
  list(FIND ARGN ${current_dir} IS_EXCLUDED)
  if (IS_EXCLUDED EQUAL -1)
    file(GLOB COLLECTED_SOURCES
        ${current_dir}/*.c
        ${current_dir}/*.cc
        ${current_dir}/*.cpp
        ${current_dir}/*.pb.cc
        )
    list(APPEND ${variable} ${COLLECTED_SOURCES})
    file(GLOB SUB_DIRECTORIES ${current_dir}/*)
    foreach (SUB_DIRECTORY ${SUB_DIRECTORIES})
      if (IS_DIRECTORY ${SUB_DIRECTORY})
        rendu_collect_source_files(${SUB_DIRECTORY} ${variable} ${ARGN})
      endif ()
    endforeach ()
    set(${variable} ${${variable}} PARENT_SCOPE)
  endif ()
endfunction()

# 将所有源文件收集到给定变量中，用于包含子目录中的所有源。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_collect_header_files(
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers
#   ${CMAKE_CURRENT_SOURCE_DIR}/platform)
#
function(rendu_collect_header_files current_dir variable)
  list(FIND ARGN ${current_dir} IS_EXCLUDED)
  if (IS_EXCLUDED EQUAL -1)
    file(GLOB COLLECTED_HEADERS
        ${current_dir}/*.h
        ${current_dir}/*.hh
        ${current_dir}/*.hpp
        ${current_dir}/*.ipp
        ${current_dir}/*.pb.h
        ${current_dir}/*.inc
        )
    list(APPEND ${variable} ${COLLECTED_HEADERS})
    file(GLOB SUB_DIRECTORIES ${current_dir}/*)
    foreach (SUB_DIRECTORY ${SUB_DIRECTORIES})
      if (IS_DIRECTORY ${SUB_DIRECTORY})
        rendu_collect_header_files(${SUB_DIRECTORY} ${variable} ${ARGN})
      endif ()
    endforeach ()
    set(${variable} ${${variable}} PARENT_SCOPE)
  endif ()
endfunction(rendu_collect_header_files)

# 将所有源文件收集到给定变量中，用于包含子目录中的所有源。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_collect_proto_files(
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   COMMON_PRIVATE_SOURCES
#   # Exclude
#   ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers
#   ${CMAKE_CURRENT_SOURCE_DIR}/platform)
#
function(rendu_collect_proto_files current_dir variable)
  list(FIND ARGN ${current_dir} IS_EXCLUDED)
  if (IS_EXCLUDED EQUAL -1)
    file(GLOB COLLECTED_SOURCES
            ${current_dir}/*.proto
            )
    list(APPEND ${variable} ${COLLECTED_SOURCES})

    file(GLOB SUB_DIRECTORIES ${current_dir}/*)
    foreach (SUB_DIRECTORY ${SUB_DIRECTORIES})
      if (IS_DIRECTORY ${SUB_DIRECTORY})
        rendu_collect_proto_files(${SUB_DIRECTORY} ${variable} ${ARGN})
      endif ()
    endforeach ()
    set(${variable} ${${variable}} PARENT_SCOPE)
  endif ()
endfunction(rendu_collect_proto_files)

# 收集所有子目录到给定的变量，用于包含所有子目录。忽略可变参数中列出的完全限定目录。
# Use it like:
# rendu_collect_include_directories(
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   COMMON_PUBLIC_INCLUDES
#   # Exclude
#   ${CMAKE_CURRENT_SOURCE_DIR}/precompiled_headers
#   ${CMAKE_CURRENT_SOURCE_DIR}/platform)
#
function(rendu_collect_include_directories current_dir variable)
  list(FIND ARGN "${current_dir}" IS_EXCLUDED)
  if (IS_EXCLUDED EQUAL -1)
    list(APPEND ${variable} ${current_dir})
    file(GLOB SUB_DIRECTORIES ${current_dir}/*)
    foreach (SUB_DIRECTORY ${SUB_DIRECTORIES})
      if (IS_DIRECTORY ${SUB_DIRECTORY})
        list(APPEND ${variable} ${SUB_DIRECTORY})
#        rendu_collect_include_directories(${SUB_DIRECTORY} ${variable} ${ARGN})
      endif ()
    endforeach ()
    set(${variable} ${${variable}} PARENT_SCOPE)
  endif ()
endfunction(rendu_collect_include_directories)



#**********************************
#  Created by boil on 2022/10/19.
#**********************************

include(CMakeParseArguments)
include(CheckCXXSourceCompiles)

#
# rendu_add_executable(
#   NAME
#     awesome_main
#   SRCS
#     "awesome_main.cc"
#   DEPS
#     rendu::awesome
#     GTest::gmock
#     GTest::gtest_main
# )

function(rendu_add_executable)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;SETTING;DIR"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(pub_exec_target "${RD_PROJECT}_${RD_NAME}")

  list(APPEND target_hdrs ${RD_HDRS})
  list(APPEND target_srcs ${RD_SRCS})

  if ("${target_srcs}${target_hdrs}" STREQUAL "")
    CollectSourceFiles(
        ${RD_DIR}
        ${target_name}_srcs
    )
    CollectHeaderFiles(
        ${RD_DIR}
        ${target_name}_hdrs
        # Exclude
        ${RD_DIR}/precompiled_headers
    )
    if (USE_PCH)
      CollectHeaderFiles(
          ${RD_DIR}/precompiled_headers
          precompiled_headers
      )
    endif (USE_PCH)
    list(APPEND target_srcs ${${target_name}_srcs})
    list(APPEND target_hdrs ${${target_name}_hdrs} ${precompiled_headers})
  endif ()

  if ("${target_srcs}${target_hdrs}" STREQUAL "")
    message(STATUS ${target_name} " can't find src files!")
  else ()
    GroupSources(${RD_DIR})
    CollectIncludeDirectories(${RD_DIR} include_dirs
        # Exclude
        ${RD_DIR}/precompiled_headers
        )

    add_executable(${pub_exec_target} "")
    target_sources(${pub_exec_target} PRIVATE ${target_hdrs} ${target_srcs})
    target_link_libraries(${pub_exec_target} PRIVATE ${RD_SETTING} PUBLIC ${RD_DEPS})
    target_include_directories(${pub_exec_target} PUBLIC ${include_dirs})
    set_target_properties(${pub_exec_target} PROPERTIES FOLDER ${RD_PROJECT})
    add_executable(${RD_PROJECT}::${RD_NAME} ALIAS ${pub_exec_target})
    # Generate precompiled header
    if (USE_PCH)
      message(STATUS "use precompiled header !")
      set(headers "${precompiled_headers}")
      if (headers STREQUAL "")
      else ()
        add_cxx_pch(${RD_PROJECT}::${RD_NAME} ${precompiled_headers})
      endif ()
    endif ()
    message(STATUS "[exec]" ${RD_PROJECT}::${RD_NAME})
  endif ()
endfunction(rendu_add_executable)

function(rendu_add_test)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;SETTING;DIR"
      "HDRS;SRCS;DEPS;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(target_name ${RD_PROJECT}_${RD_NAME})
  set(src_dir ${RD_DIR})

  message(STATUS "[test] " ${target_name})

  CollectAllFiles(
      ${src_dir}
      ${target_name}_srcs
      # Exclude
      ${src_dir}/precompiled_headers
  )

  if (USE_PCH)
    CollectHeaderFiles(
        ${src_dir}/precompiled_headers
        precompiled_headers
    )
  endif (USE_PCH)

  list(APPEND target_srcs
      ${${target_name}_srcs}
      ${precompiled_headers}
      )

  GroupSources(${src_dir})

  add_executable(${target_name} "")
  target_sources(${target_name} PRIVATE ${target_srcs})
  target_link_libraries(${target_name}
      PRIVATE
      ${RD_SETTING}
      PUBLIC
      ${RD_DEPS}
      )

  CollectIncludeDirectories(
      ${src_dir}
      include_dirs
      # Exclude
      ${src_dir}/precompiled_headers)

  target_include_directories(${target_name}
      PRIVATE
      ${include_dirs}
      )

  #  set_target_properties(${target_name}
  #      PROPERTIES
  #      FOLDER
  #      ${RD_EXEC_NAME})


  # Generate precompiled header
  if (USE_PCH)
    message(STATUS "use precompiled header !")
    set(headers "${precompiled_headers}")
    if (headers STREQUAL "")
    else ()
      add_cxx_pch(${target_name} ${precompiled_headers})
    endif ()
  endif ()

  add_test(NAME ${target_name} COMMAND ${target_name})

endfunction(rendu_add_test)

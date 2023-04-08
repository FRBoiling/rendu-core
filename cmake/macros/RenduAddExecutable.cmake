#**********************************
#  Created by boil on 2022/10/19.
#**********************************
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
  cmake_parse_arguments(RD_EXEC
      ""
      "PROJECT;NAME;DIR"
      "HDRS;SRCS;DEPS;LINKOPTS;DEFINES;COPTS"
      ${ARGN}
      )
  set(RD_EXEC_TARGET "${RD_EXEC_PROJECT}_${RD_EXEC_NAME}")

  list(APPEND RD_TARGET_HDRS ${RD_EXEC_HDRS})
  if ("${RD_TARGET_HDRS}" STREQUAL "")
    rendu_collect_header_files(
        ${RD_EXEC_HDRS}
        RD_TARGET_HDRS
        # Exclude
        ${RD_EXEC_DIR}/precompiled_headers
    )
    if (USE_PCH)
      rendu_collect_header_files(
          ${RD_EXEC_DIR}/precompiled_headers
          RD_PCH_HEADERS
      )
    endif (USE_PCH)
  endif ()

  list(APPEND RD_TARGET_SRCS ${RD_EXEC_SRCS})
  if ("${RD_TARGET_SRCS}" STREQUAL "")
    rendu_collect_source_files(
        ${RD_EXEC_DIR}
        RD_TARGET_SRCS
    )
  endif ()

  if ("${RD_TARGET_HDRS}${RD_TARGET_SRCS}" STREQUAL "")
    message(STATUS ${RD_EXEC_TARGET} " can't find src files!")
  else ()
    rendu_source_groups(${RD_EXEC_DIR})
    rendu_collect_include_directories(${RD_EXEC_DIR}
        RD_EXEC_INCLUDES
        # Exclude
        ${RD_EXEC_DIR}/precompiled_headers
        )

    add_executable(${RD_EXEC_TARGET} "")
    target_sources(${RD_EXEC_TARGET} PRIVATE ${RD_TARGET_HDRS} ${RD_TARGET_SRCS})
    target_include_directories(${RD_EXEC_TARGET} PUBLIC ${RD_EXEC_INCLUDES})
    target_compile_options(${RD_EXEC_TARGET} PUBLIC ${RD_EXEC_COPTS})
    target_compile_definitions(${RD_EXEC_TARGET} PUBLIC ${RD_EXEC_DEFINES})
    target_link_libraries(${RD_EXEC_TARGET} PRIVATE ${RD_EXEC_LINKOPTS} PUBLIC ${RD_EXEC_DEPS})
    set_target_properties(${RD_EXEC_TARGET} PROPERTIES FOLDER ${RD_EXEC_PROJECT})
    
    add_executable(${RD_EXEC_PROJECT}::${RD_EXEC_NAME} ALIAS ${RD_EXEC_TARGET})
    # Generate precompiled header
    if (USE_PCH)
      message(STATUS "use precompiled header !")
      if ("${RD_PCH_HEADERS}" STREQUAL "")
      else ()
        add_cxx_pch(${RD_EXEC_PROJECT}::${RD_EXEC_NAME} ${RD_PCH_HEADERS})
      endif ()
    endif ()
    message(STATUS "[exec]" ${RD_EXEC_PROJECT}::${RD_EXEC_NAME})
  endif ()
endfunction(rendu_add_executable)

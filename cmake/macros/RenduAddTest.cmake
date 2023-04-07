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

function(rendu_add_test)
  cmake_parse_arguments(RD_TEST
      ""
      "PROJECT;NAME;DIR"
      "HDRS;SRCS;DEPS;LINKOPTS;DEFINES;COPTS"
      ${ARGN}
      )
  set(RD_TEST_TARGET "${RD_TEST_PROJECT}_${RD_TEST_NAME}")

  list(APPEND RD_TARGET_HDRS ${RD_TEST_HDRS})
  if ("${RD_TARGET_HDRS}" STREQUAL "")
    CollectHeaderFiles(
        ${RD_TEST_HDRS}
        RD_TARGET_HDRS
        # Exclude
        ${RD_TEST_DIR}/precompiled_headers
    )
    if (USE_PCH)
      CollectHeaderFiles(
          ${RD_TEST_DIR}/precompiled_headers
          RD_PCH_HEADERS
      )
    endif (USE_PCH)
  endif ()

  list(APPEND RD_TARGET_SRCS ${RD_TEST_SRCS})
  if ("${RD_TARGET_SRCS}" STREQUAL "")
    CollectSourceFiles(
        ${RD_TEST_DIR}
        RD_TARGET_SRCS
    )
  endif ()

  if ("${RD_TARGET_HDRS}${RD_TARGET_SRCS}" STREQUAL "")
    message(STATUS ${RD_TEST_TARGET} " can't find src files!")
  else ()
    GroupSources(${RD_TEST_DIR})
    CollectIncludeDirectories(${RD_TEST_DIR}
        RD_TEST_INCLUDES
        # Exclude
        ${RD_TEST_DIR}/precompiled_headers
        )

    add_executable(${RD_TEST_TARGET} "")
    target_sources(${RD_TEST_TARGET} PRIVATE ${RD_TARGET_HDRS} ${RD_TARGET_SRCS})
    target_include_directories(${RD_TEST_TARGET} PUBLIC ${RD_TEST_INCLUDES})
    target_compile_options(${RD_TEST_TARGET} PUBLIC ${RD_TEST_COPTS})
    target_compile_definitions(${RD_TEST_TARGET} PUBLIC ${RD_TEST_DEFINES})
    target_link_libraries(${RD_TEST_TARGET} PRIVATE ${RD_TEST_LINKOPTS} PUBLIC ${RD_TEST_DEPS})
    set_target_properties(${RD_TEST_TARGET} PROPERTIES FOLDER ${RD_TEST_PROJECT})

    add_executable(${RD_TEST_PROJECT}::${RD_TEST_NAME} ALIAS ${RD_TEST_TARGET})
    # Generate precompiled header
    if (USE_PCH)
      message(STATUS "use precompiled header !")
      if ("${RD_PCH_HEADERS}" STREQUAL "")
      else ()
        add_cxx_pch(${RD_TEST_PROJECT}::${RD_TEST_NAME} ${RD_PCH_HEADERS})
      endif ()
    endif ()
  endif ()
  message(STATUS "[test]" ${RD_TEST_PROJECT}::${RD_TEST_NAME})

  add_test(NAME ${RD_TEST_TARGET} COMMAND ${RD_TEST_TARGET})
  
endfunction(rendu_add_test)

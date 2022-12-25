#|--------------------------------
#| RD Application Project
#|--------------------------------

set(target-name fmt)
set(header_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(sources_dir ${CMAKE_CURRENT_SOURCE_DIR})
CollectSourceFiles(
    ${sources_dir}
    ${target-name}_sources
    # Exclude
)

CollectHeaderFiles(
    ${header_dir}
    ${target-name}_headers
    # Exclude

)
set(RENDU_PROPAGATE_CXX_STD ON)
rendu_cc_test(${project-name}
    NAME
    ${target-name}
    HDRS
    ${${target-name}_headers}
    SRCS
    ${${target-name}_sources}
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    fmt::fmt-header-only
    fmt::fmt
    GTest::gmock_main
    GTest::gtest_main
    )

#|--------------------------------
#| RD Application Project
#|--------------------------------

set(project-name examples)
rendu_cc_executable(${project-name} algorithm
    NAME
    algorithm
    HDRS
    SRCS
    example_algorithm.cpp
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::algorithm
    absl::config
    PUBLIC
    )

#|--------------------------------
#| rendu-core rendu project
#|--------------------------------

set(PROJECT_NAME engine)
CollectSourceFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    RENDU_LIB_SOURCE
)
CollectHeaderFiles(
    ${CMAKE_CURRENT_SOURCE_DIR}
    RENDU_LIB_HEADERS
)

#|---------------------------------------------
#| rendu Framework Interface target and alias
#|---------------------------------------------
add_library(
    rendu
    ${RENDU_LIB_HEADERS}
    ${RENDU_LIB_SOURCE}
)
add_library(rendu::rendu ALIAS rendu)

#|--------------------------------
#| rendu Framework Dependencies
#|--------------------------------
target_include_directories(rendu INTERFACE ${RENDU_PROJECT_DIR})
if(RENDU_FLAG_USE_BUNDLED_DEPS)
  target_include_directories(rendu INTERFACE ${RENDU_PROJECT_DIR}/deps)
else()
  target_link_libraries(rendu INTERFACE fmt::fmt-header-only)
endif()

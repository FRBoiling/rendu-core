#|--------------------------------
#| RD Application Project
#|--------------------------------
cmake_minimum_required(VERSION 3.14)

set(RENDU_APP ${tests_key}_catch2)
project(${RENDU_APP})

if (RENDU_FLAG_USE_BUNDLED_DEPS)
  include(FetchContent)
  FetchContent_Declare(Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v2.13.9
      GIT_SHALLOW 1)
  FetchContent_MakeAvailable(Catch2)
endif ()

include(CTest)
list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/contrib")
include(Catch)

file(GLOB_RECURSE RENDU_APP_SOURCE *.cpp *.cc *.c)
file(GLOB_RECURSE RENDU_APP_HEADERS *.h *.hpp *.ipp)

add_executable(${RENDU_APP} ${RENDU_APP_SOURCE} ${RENDU_APP_HEADERS})
target_link_libraries(${RENDU_APP} PRIVATE Catch2::Catch2 rendu::rendu)


catch_discover_tests(${RENDU_APP})

source_group("Source" FILES ${RENDU_APP_SOURCE})
source_group("Headers" FILES ${RENDU_APP_HEADERS})

if (WIN32)
  set_target_properties(${RENDU_APP} PROPERTIES LINK_FLAGS "/DEBUG /PDBSTRIPPED:${RENDU_APP}.pdb")

  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")  # Use /Zc:preprocessor for support VA_OPT in MSVC
  endif ()
endif ()

#**********************************
#  Created by boil on 2022/10/19.
#**********************************

include(CMakeParseArguments)

set(RENDU_INTERNAL_DLL_FILES
)

set(RENDU_INTERNAL_DLL_TARGETS
  
)

function(rendu_internal_dll_contains)
  cmake_parse_arguments(RENDU_INTERNAL_DLL
    ""
    "OUTPUT;TARGET"
    ""
    ${ARGN}
  )

  STRING(REGEX REPLACE "^rendu::" "" _target ${RENDU_INTERNAL_DLL_TARGET})

  list(FIND
    RENDU_INTERNAL_DLL_TARGETS
    "${_target}"
    _index)

  if (${_index} GREATER -1)
    set(${RENDU_INTERNAL_DLL_OUTPUT} 1 PARENT_SCOPE)
  else()
    set(${RENDU_INTERNAL_DLL_OUTPUT} 0 PARENT_SCOPE)
  endif()
endfunction()

function(rendu_internal_dll_targets)
  cmake_parse_arguments(RENDU_INTERNAL_DLL
  ""
  "OUTPUT"
  "DEPS"
  ${ARGN}
  )

  set(_deps "")
  foreach(dep IN LISTS RENDU_INTERNAL_DLL_DEPS)
    rendu_internal_dll_contains(TARGET ${dep} OUTPUT _contains)
    if (_contains)
      list(APPEND _deps abseil_dll)
    else()
      list(APPEND _deps ${dep})
    endif()
  endforeach()

  # Because we may have added the DLL multiple times
  list(REMOVE_DUPLICATES _deps)
  set(${RENDU_INTERNAL_DLL_OUTPUT} "${_deps}" PARENT_SCOPE)
endfunction()

function(rendu_make_dll name)
  set(dll_name ${name}_dll)
  add_library(${dll_name}
    SHARED
      "${RENDU_INTERNAL_DLL_FILES}"
  )
  target_link_libraries(${dll_name}
    PRIVATE
      ${RENDU_DEFAULT_LINKOPTS}
  )
  set_property(TARGET abseil_dll PROPERTY LINKER_LANGUAGE "CXX")
  target_include_directories(
    abseil_dll
    PUBLIC
      "$<BUILD_INTERFACE:${RENDU_COMMON_INCLUDE_DIRS}>"
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  target_compile_options(${dll_name}
    PRIVATE
      ${RENDU_DEFAULT_COPTS}
  )

  target_compile_definitions(${dll_name}
    PRIVATE
      RENDU_BUILD_DLL
      NOMINMAX
    INTERFACE
      ${RENDU_CC_LIB_DEFINES}
      RENDU_CONSUME_DLL
  )
  install(TARGETS ${dll_name}
      EXPORT ${PROJECT_NAME}Targets
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
endfunction()

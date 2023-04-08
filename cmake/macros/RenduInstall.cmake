#**********************************
#  Created by boil on 2022/10/19.
#**********************************

function(rendu_export_and_install_lib alias component)
  string(TOUPPER "${component}_EXPORT" export_macro_name)
  target_include_directories(${component} INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
      $<INSTALL_INTERFACE:include>)
  generate_export_header(
      ${component}
      EXPORT_MACRO_NAME ${export_macro_name}
      EXPORT_FILE_NAME "caf/detail/${component}_export.hpp")
  set_target_properties(${component} PROPERTIES
      EXPORT_NAME ${component}
      SOVERSION ${RD_VERSION}
      VERSION ${RD_LIB_VERSION}
      OUTPUT_NAME caf_${component})
  install(TARGETS ${component}
      EXPORT RDTargets
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${component}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${component}
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${component})
  install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/caf"
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      COMPONENT ${component}
      FILES_MATCHING PATTERN "*.hpp")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/caf/detail/${component}_export.hpp"
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/caf/detail/")
endfunction()
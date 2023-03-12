set(libprotoc_files
  ${protobuf_source_dir}/src/google/protobuf/compiler/code_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/command_line_interface.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_enum.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_enum_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_extension.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_file.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_helpers.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_map_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_message.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_message_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_padding_optimizer.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_parse_function_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_primitive_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_service.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_string_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_doc_comment.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_enum.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_enum_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_field_base.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_helpers.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_map_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_message.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_message_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_primitive_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_reflection_class.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_repeated_enum_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_repeated_message_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_repeated_primitive_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_source_generator_base.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_wrapper_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_context.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_doc_comment.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_enum.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_enum_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_enum_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_enum_field_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_enum_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_extension.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_extension_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_file.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_generator_factory.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_helpers.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_kotlin_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_map_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_map_field_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_message.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_message_builder.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_message_builder_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_message_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_message_field_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_message_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_name_resolver.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_primitive_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_primitive_field_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_service.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_shared_code_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_string_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_string_field_lite.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/js/js_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/js/well_known_types_embed.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_enum.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_enum.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_enum_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_enum_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_extension.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_extension.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_file.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_file.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_helpers.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_map_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_map_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_message.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_message.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_message_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_message_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_nsobject_methods.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_oneof.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_oneof.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_primitive_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_primitive_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/php/php_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/python/python_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/python/python_helpers.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/python/python_pyi_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/ruby/ruby_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/subprocess.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/zip_writer.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/zip_writer.h
)

set(libprotoc_headers
  ${protobuf_source_dir}/src/google/protobuf/compiler/code_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/command_line_interface.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_file.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_helpers.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_names.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_doc_comment.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_names.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/csharp/csharp_options.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_kotlin_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/java/java_names.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/js/js_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/objectivec/objectivec_helpers.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/php/php_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/python/python_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/python/python_pyi_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/ruby/ruby_generator.h
)

#if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#set(libprotoc_rc_files
#  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
#)
#endif()
#
add_library(libprotoc
  ${libprotoc_files} ${libprotoc_headers})
#if(protobuf_HAVE_LD_VERSION_SCRIPT)
#  if(${CMAKE_VERSION} VERSION_GREATER 3.13 OR ${CMAKE_VERSION} VERSION_EQUAL 3.13)
#    target_link_options(libprotoc PRIVATE -Wl,--version-script=${protobuf_source_dir}/src/libprotoc.map)
#  elseif(protobuf_BUILD_SHARED_LIBS)
#    target_link_libraries(libprotoc PRIVATE -Wl,--version-script=${protobuf_source_dir}/src/libprotoc.map)
#  endif()
#  set_target_properties(libprotoc PROPERTIES
#    LINK_DEPENDS ${protobuf_source_dir}/src/libprotoc.map)
#endif()
target_link_libraries(libprotoc PRIVATE libprotobuf)
#if(protobuf_BUILD_SHARED_LIBS)
#  target_compile_definitions(libprotoc
#    PUBLIC  PROTOBUF_USE_DLLS
#    PRIVATE LIBPROTOC_EXPORTS)
#endif()
#
add_library(protobuf::libprotoc ALIAS libprotoc)

#rendu_add_library(
#    NAME
#    libprotoc
#    SRC_DIR
#    ${protobuf_source_dir}
#    SRCS
#    ${libprotoc_files}
#    HDRS
#    ${libprotoc_headers}
#    PRIVATE
#    rendu-dependency-interface
#    DEPS
#    DEFINES
#    LIBPROTOC_EXPORTS
#    LINKOPTS
#)
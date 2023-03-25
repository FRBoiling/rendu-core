set(libprotobuf_lite_files
    ${protobuf_source_dir}/src/google/protobuf/any_lite.cc
    ${protobuf_source_dir}/src/google/protobuf/arena.cc
    ${protobuf_source_dir}/src/google/protobuf/arenastring.cc
    ${protobuf_source_dir}/src/google/protobuf/arenaz_sampler.cc
    ${protobuf_source_dir}/src/google/protobuf/extension_set.cc
    ${protobuf_source_dir}/src/google/protobuf/generated_enum_util.cc
    ${protobuf_source_dir}/src/google/protobuf/generated_message_tctable_lite.cc
    ${protobuf_source_dir}/src/google/protobuf/generated_message_util.cc
    ${protobuf_source_dir}/src/google/protobuf/implicit_weak_message.cc
    ${protobuf_source_dir}/src/google/protobuf/inlined_string_field.cc
    ${protobuf_source_dir}/src/google/protobuf/io/coded_stream.cc
    ${protobuf_source_dir}/src/google/protobuf/io/io_win32.cc
    ${protobuf_source_dir}/src/google/protobuf/io/strtod.cc
    ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream.cc
    ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl.cc
    ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
    ${protobuf_source_dir}/src/google/protobuf/map.cc
    ${protobuf_source_dir}/src/google/protobuf/message_lite.cc
    ${protobuf_source_dir}/src/google/protobuf/parse_context.cc
    ${protobuf_source_dir}/src/google/protobuf/repeated_field.cc
    ${protobuf_source_dir}/src/google/protobuf/repeated_ptr_field.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/bytestream.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/common.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/int128.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/status.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/statusor.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/stringpiece.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/stringprintf.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/structurally_valid.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/strutil.cc
    ${protobuf_source_dir}/src/google/protobuf/stubs/time.cc
    ${protobuf_source_dir}/src/google/protobuf/wire_format_lite.cc
    )

set(libprotobuf_lite_includes
    ${protobuf_source_dir}/src/google/protobuf/any.h
    ${protobuf_source_dir}/src/google/protobuf/arena.h
    ${protobuf_source_dir}/src/google/protobuf/arena_impl.h
    ${protobuf_source_dir}/src/google/protobuf/arenastring.h
    ${protobuf_source_dir}/src/google/protobuf/arenaz_sampler.h
    ${protobuf_source_dir}/src/google/protobuf/explicitly_constructed.h
    ${protobuf_source_dir}/src/google/protobuf/extension_set.h
    ${protobuf_source_dir}/src/google/protobuf/extension_set_inl.h
    ${protobuf_source_dir}/src/google/protobuf/generated_enum_util.h
    ${protobuf_source_dir}/src/google/protobuf/generated_message_tctable_decl.h
    ${protobuf_source_dir}/src/google/protobuf/generated_message_tctable_impl.h
    ${protobuf_source_dir}/src/google/protobuf/generated_message_util.h
    ${protobuf_source_dir}/src/google/protobuf/has_bits.h
    ${protobuf_source_dir}/src/google/protobuf/implicit_weak_message.h
    ${protobuf_source_dir}/src/google/protobuf/inlined_string_field.h
    ${protobuf_source_dir}/src/google/protobuf/io/coded_stream.h
    ${protobuf_source_dir}/src/google/protobuf/io/io_win32.h
    ${protobuf_source_dir}/src/google/protobuf/io/strtod.h
    ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream.h
    ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl.h
    ${protobuf_source_dir}/src/google/protobuf/io/zero_copy_stream_impl_lite.h
    ${protobuf_source_dir}/src/google/protobuf/map.h
    ${protobuf_source_dir}/src/google/protobuf/map_entry_lite.h
    ${protobuf_source_dir}/src/google/protobuf/map_field_lite.h
    ${protobuf_source_dir}/src/google/protobuf/map_type_handler.h
    ${protobuf_source_dir}/src/google/protobuf/message_lite.h
    ${protobuf_source_dir}/src/google/protobuf/metadata_lite.h
    ${protobuf_source_dir}/src/google/protobuf/parse_context.h
    ${protobuf_source_dir}/src/google/protobuf/port.h
    ${protobuf_source_dir}/src/google/protobuf/repeated_field.h
    ${protobuf_source_dir}/src/google/protobuf/repeated_ptr_field.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/bytestream.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/callback.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/casts.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/common.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/hash.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/logging.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/macros.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/map_util.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/mutex.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/once.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/platform_macros.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/port.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/status.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/stl_util.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/stringpiece.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/strutil.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/template_util.h
    ${protobuf_source_dir}/src/google/protobuf/wire_format_lite.h
    ${protobuf_source_dir}/src/google/protobuf/stubs/time.h
    )

rendu_add_component(
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
    protobuf-lite
    HDRS
    ${libprotobuf_lite_includes}
    SRCS
    ${libprotobuf_lite_files}
    SETTING
    rendu-dependency-interface
    DEFINES
    DEPS
    dep::threads
    LINKOPTS
)
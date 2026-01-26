#**********************************
# RenduCore 项目 - Protobuf C++ 编译配置
#**********************************

set(PROTOBUF_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# ====================================================================
# 加载 Protobuf 生成宏
# ====================================================================
include(${PROTOBUF_SRC_DIR}/cmake/protobuf-generate.cmake)

# ====================================================================
# 收集 Protobuf 源文件
# ====================================================================

# libprotobuf 核心源文件
set(PROTOBUF_SOURCES
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/any.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/any_lite.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/arena.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/arena_align.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/arenastring.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/arenaz_sampler.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/descriptor.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/descriptor_database.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/dynamic_message.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/extension_set.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/extension_set_heavy.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/feature_resolver.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/generated_enum_util.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/generated_message_bases.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/generated_message_reflection.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/generated_message_tctable_full.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/generated_message_tctable_gen.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/generated_message_tctable_lite.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/generated_message_util.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/implicit_weak_message.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/inlined_string_field.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/internal_feature_helper.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/coded_stream.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/gzip_stream.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/io_win32.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/printer.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/strtod.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/tokenizer.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/zero_copy_sink.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/zero_copy_stream.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/zero_copy_stream_impl.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/internal/lexer.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/internal/message_path.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/internal/parser.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/internal/unparser.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/internal/untyped_message.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/internal/writer.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/internal/zero_copy_buffered_stream.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/json/json.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/map.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/map_field.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/message.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/message_lite.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/micro_string.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/parse_context.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/port.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/raw_ptr.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/reflection_mode.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/reflection_ops.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/repeated_field.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/repeated_ptr_field.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/service.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/stubs/common.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/text_format.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/unknown_field_set.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/util/delimited_message_util.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/util/field_comparator.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/util/field_mask_util.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/util/message_differencer.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/util/time_util.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/util/type_resolver_util.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/wire_format.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/wire_format_lite.cc
)

# 预生成的 protobuf 源文件
set(PROTOBUF_GENERATED_SOURCES
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/any.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/api.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/duration.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/empty.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/field_mask.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/source_context.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/struct.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/timestamp.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/type.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/wrappers.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/cpp_features.pb.cc
        ${PROTOBUF_SRC_DIR}/src/google/protobuf/descriptor.pb.cc
)

# 检查源文件是否存在并合并列表
set(EXISTING_PROTOBUF_SOURCES "")
foreach (SRC_FILE IN LISTS PROTOBUF_SOURCES)
    if (EXISTS ${SRC_FILE})
        list(APPEND EXISTING_PROTOBUF_SOURCES ${SRC_FILE})
    endif ()
endforeach ()
foreach (SRC_FILE IN LISTS PROTOBUF_GENERATED_SOURCES)
    if (EXISTS ${SRC_FILE})
        list(APPEND EXISTING_PROTOBUF_SOURCES ${SRC_FILE})
    endif ()
endforeach ()

# ====================================================================
# 使用 rendu_add_library 编译
# ====================================================================

if (EXISTING_PROTOBUF_SOURCES)
    get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

    rendu_add_library(
            DIR ${PROTOBUF_SRC_DIR}
            PROJECT ${PROJECT_NAME}
            NAME ${FOLDER_NAME}
            SOURCES ${EXISTING_PROTOBUF_SOURCES}
            DEFINES
            GOOGLE_PROTOBUF_NO_RTTI
            GOOGLE_PROTOBUF_OPTIMIZE_FOR_SPEED
            PRIVATE_LINK rendu-dependency-interface
            PUBLIC_LINK 3rdparty::absl 3rdparty::utf8_range
            ALIAS ON
    )

    # 手动添加 utf8_range 的 include 目录，因为 protobuf 需要 #include "utf8_validity.h"
    get_target_property(TARGET_NAME 3rdparty_${FOLDER_NAME} ALIAS_GLOBAL)
    if(NOT TARGET_NAME)
        set(TARGET_NAME 3rdparty_${FOLDER_NAME})
    endif()
    target_include_directories(${TARGET_NAME}
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../utf8_range/utf8_range
    )
else ()
    message(WARNING "No Protobuf source files found. Please ensure Protobuf C++ sources are available in ${PROTOBUF_SRC_DIR}/src/google/protobuf/")
endif ()

# ====================================================================
# 可选：编译 upb
# ====================================================================

option(BUILD_UPB "Build upb (micro protobuf library)" OFF)

if (BUILD_UPB)
    file(GLOB_RECURSE UPB_SOURCES
            "${PROTOBUF_SRC_DIR}/upb/*.c"
            "${PROTOBUF_SRC_DIR}/upb/*.cc"
    )

    if (UPB_SOURCES)
        rendu_add_library(
                DIR ${PROTOBUF_SRC_DIR}/upb
                PROJECT ${PROJECT_NAME}
                NAME upb
                SOURCES ${UPB_SOURCES}
                PRIVATE_LINK rendu-dependency-interface
                PUBLIC_LINK 3rdparty::absl 3rdparty::utf8_range
                ALIAS ON
        )
    endif ()
endif ()

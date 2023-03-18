#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(PROTOBUF_PROTOC_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tool_proto_generator)
set(proto_model_output_dir ${CMAKE_CURRENT_SOURCE_DIR}/../cxx)

set(proto_import_dir ${RENDU_PROJECT_DIR}/dep/protobuf/src)
set(proto_input_dir ${CMAKE_CURRENT_SOURCE_DIR})

if(EXISTS ${proto_model_output_dir} AND IS_DIRECTORY ${proto_model_output_dir})
else()
  file(MAKE_DIRECTORY ${proto_model_output_dir})
endif()

#获取需要编译的proto文件
CollectProtoFiles(
    ${proto_import_dir}
    RD_PROTO_FILES
)

CollectProtoFiles(
    ${proto_input_dir}
    RD_PROTO_FILES
)
#输出的文件列表
set(OutPut_SRCS "")
set(OutPut_HDRS "")

foreach(proto_file ${RD_PROTO_FILES})
  get_filename_component(FIL_WE ${proto_file} NAME_WE)

  set(OutPut_SRC ${CMAKE_BINARY_DIR}/proto/${FIL_WE}.pb.cc)
  set(OutPut_HDR ${CMAKE_BINARY_DIR}/proto/${FIL_WE}.pb.h)

  list(APPEND OutPut_SRCS ${OutPut_SRC})
  list(APPEND OutPut_HDRS ${OutPut_HDR})

  # 使用自定义命令
  add_custom_command(
      OUTPUT ${OutPut_SRC} ${OutPut_HDR}
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ARGS
      --cpp_out ${proto_model_output_dir}
      -I${proto_import_dir} -I${proto_input_dir} ${proto_file}
      DEPENDS ${proto_file}
      COMMENT "Running C++ protocol buffer compiler on ${proto_file}"
      VERBATIM
  )

endforeach()

# 设置文件属性为 GENERATED
set_source_files_properties(${OutPut_SRCS} ${OutPut_HDRS} PROPERTIES GENERATED TRUE)

# 添加自定义target
add_custom_target(generate_model ALL
    DEPENDS ${OutPut_SRCS} ${OutPut_HDRS}
    COMMENT "generate model target"
    VERBATIM
    )

## 拷贝文件夹
#execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${proto_model_output_dir}/google ${CMAKE_CURRENT_SOURCE_DIR}/test_out/google/)

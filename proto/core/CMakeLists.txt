#**********************************
#  Created by boil on 2022/8/14.
#**********************************
set(target-name generate_model)
#set(project-name deps)
#设置输入路径
set(input_dir ${CMAKE_CURRENT_SOURCE_DIR})
#设置输出路径
set(output_dir ${CMAKE_BINARY_DIR}/proto/core)
message(---->${CMAKE_BINARY_DIR}/proto/core)
if(EXISTS ${output_dir} AND IS_DIRECTORY ${output_dir})
else()
  file(MAKE_DIRECTORY ${output_dir})
endif()
#获取需要编译的proto文件
CollectProtoFiles(
    ${input_dir}
    PROTO_FILES
)
#输出的文件列表
set(OutPut_SRCS "")
set(OutPut_HDRS "")

foreach(proto_file ${PROTO_FILES})
  get_filename_component(FIL_WE ${proto_file} NAME_WE)

  set(OutPut_SRC ${output_dir}/${FIL_WE}.pb.cc)
  set(OutPut_HDR ${output_dir}/${FIL_WE}.pb.h)

  list(APPEND OutPut_SRCS ${OutPut_SRC})
  list(APPEND OutPut_HDRS ${OutPut_HDR})

  # 使用自定义命令
  add_custom_command(
      OUTPUT ${OutPut_SRC} ${OutPut_HDR}
      COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE} ARGS --cpp_out ${output_dir} -I ${input_dir} ${proto_file}
      DEPENDS ${proto_file}
      COMMENT "Running C++ protocol buffer compiler on ${proto_file}"
      VERBATIM
  )
endforeach()

# 设置文件属性为 GENERATED
set_source_files_properties(${OutPut_SRCS} ${OutPut_HDRS} PROPERTIES GENERATED TRUE)

# 添加自定义target
add_custom_target(${target-name} ALL
    DEPENDS ${OutPut_SRCS} ${OutPut_HDRS}
    COMMENT "generate core model target"
    VERBATIM
    )



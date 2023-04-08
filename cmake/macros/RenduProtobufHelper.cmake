function(rendu_proto_cxx)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;PROTOC"
      "SRCS;INPUT_DIRS;OUTPUT_DIR;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(target_name ${RD_PROJECT}_${RD_NAME})
  list(APPEND input_str "")
  if (EXISTS ${RD_OUTPUT_DIR} AND IS_DIRECTORY ${RD_OUTPUT_DIR})
    foreach (dir ${RD_INPUT_DIRS})
      rendu_collect_proto_files(
          ${dir}
          proto_files
      )
      list(APPEND input_str "-I${dir}")
    endforeach ()

    set(temp_op_srcs "")
    foreach (proto_file ${proto_files})
      get_filename_component(FIL_WE ${proto_file} NAME_WE)

      set(temp_src ${CMAKE_BINARY_DIR}/proto/${FIL_WE}.pb.cc)
      set(temp_hdr ${CMAKE_BINARY_DIR}/proto/${FIL_WE}.pb.h)

      list(APPEND temp_op_srcs ${temp_src})
      list(APPEND temp_op_hdrs ${temp_hdr})

      # 使用自定义命令
      add_custom_command(
          OUTPUT ${temp_src} ${temp_hdr}
          #      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ARGS
          COMMAND ${RD_PROTOC} ARGS
          --cpp_out ${RD_OUTPUT_DIR}
          ${input_str} ${proto_file}
          DEPENDS ${proto_file}
          COMMENT "Running C++ protocol buffer compiler on ${proto_file}"
          VERBATIM
      )
    endforeach ()

    # 设置文件属性为 GENERATED
    set_source_files_properties(${temp_op_srcs} ${temp_op_hdrs} PROPERTIES GENERATED TRUE)

    # 添加自定义target
    add_custom_target(${target_name} ALL
        DEPENDS ${temp_op_srcs} ${temp_op_hdrs}
        COMMENT "generate protobuf ${target_name} target"
        VERBATIM
        )
    ## 拷贝文件夹
    #execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${proto_model_output_dir}/google ${CMAKE_CURRENT_SOURCE_DIR}/test_out/google/)
  else ()
    file(MAKE_DIRECTORY ${RD_OUTPUT_DIR})
  endif ()

endfunction(rendu_proto_cxx)

function(rendu_proto_csharp)
  cmake_parse_arguments(RD
      ""
      "PROJECT;NAME;PROTOC"
      "SRCS;INPUT_DIRS;OUTPUT_DIR;COPTS;DEFINES;LINKOPTS;PUBLIC;PRIVATE"
      ${ARGN}
      )
  set(target_name ${RD_NAME})
  list(APPEND input_str "")
  if (EXISTS ${RD_OUTPUT_DIR} AND IS_DIRECTORY ${RD_OUTPUT_DIR})
    foreach (dir ${RD_INPUT_DIRS})
      rendu_collect_proto_files(
          ${dir}
          proto_files
      )
      list(APPEND input_str "-I${dir}")
    endforeach ()

    set(temp_op_srcs "")
    foreach (proto_file ${proto_files})
      get_filename_component(FIL_WE ${proto_file} NAME_WE)
      set(temp_src ${CMAKE_BINARY_DIR}/proto/${FIL_WE}.cs)

      list(APPEND temp_op_srcs ${temp_src})

      # 使用自定义命令
      add_custom_command(
          OUTPUT ${temp_src}
          #      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ARGS
          COMMAND ${RD_PROTOC} ARGS
          --csharp_out ${RD_OUTPUT_DIR}
          ${input_str} ${proto_file}
#          --csharp_out ${proto_model_output_dir}
#          -I${proto_import_dir} -I${proto_input_dir} ${proto_file}
          DEPENDS ${proto_file}
          COMMENT "Running CSharp protocol buffer compiler on ${proto_file}"
          VERBATIM
      )
    endforeach ()

    # 设置文件属性为 GENERATED
    set_source_files_properties(${temp_op_srcs} PROPERTIES GENERATED TRUE)

    # 添加自定义target
    add_custom_target(${target_name} ALL
        DEPENDS ${temp_op_srcs}
        COMMENT "generate ${target_name} target"
        VERBATIM
        )
  else ()
    file(MAKE_DIRECTORY ${RD_OUTPUT_DIR})
  endif ()

endfunction(rendu_proto_csharp)
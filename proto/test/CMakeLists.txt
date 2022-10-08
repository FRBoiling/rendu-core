#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(source_dir ${PROTO_ROOT}/test)
set(output_dir ${PROTO_ROOT}/test)

CollectProtoFiles(
    ${source_dir}
    private_sources
)

foreach(fil ${private_sources})
  get_filename_component(abs_fil ${fil} ABSOLUTE)
  get_filename_component(fil_we ${fil} NAME_WE)

  list(APPEND srcs_var ${fil_we}.pb.cc)
  list(APPEND hdrs_var ${fil_we}.pb.h)

  add_custom_command(
      OUTPUT ${fil_we}.pb.cc ${fil_we}.pb.h
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out ${output_dir} -I${source_dir} ${abs_fil}
      DEPENDS ${abs_fil}
      COMMENT "Running C++ protocol buffer compiler on ${fil}" VERBATIM )
endforeach()

add_library(proto_test_generate_out ${srcs_var} ${hdrs_var})
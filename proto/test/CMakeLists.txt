#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(source_dir ${PROTO_ROOT}/test)
set(output_dir ${PROTO_ROOT}/test_out)
set(output_path ${PROTO_ROOT}/test_out/google/protobuf)

CollectProtoFiles(
    ${source_dir}
    private_sources
)

foreach(fil ${private_sources})
  get_filename_component(abs_fil ${fil} ABSOLUTE)
  get_filename_component(fil_we ${fil} NAME_WE)

  add_custom_target(${fil_we}
      OUTPUT ${fil_we}.pb.h ${fil_we}.pb.cc
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out ${output_dir} -I${source_dir} ${abs_fil}
      DEPENDS ${abs_fil}
      COMMENT "Running C++ protocol buffer compiler on ${fil}" VERBATIM )

  list(APPEND hdrs_var ${output_path}/${fil_we}.pb.h)
  list(APPEND srcs_var ${output_path}/${fil_we}.pb.cc)
endforeach()

set(target-name proto_test_generate_out)
add_library(${target-name} ${hdrs_var} ${srcs_var})

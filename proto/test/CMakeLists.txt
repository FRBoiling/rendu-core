#**********************************
#  Created by boil on 2022/8/14.
#**********************************


set(output_dir ${PROTO_ROOT}/out)
set(protoc_include ${PROTO_ROOT}/test/unittest)
set(protoc_sources ${PROTO_ROOT}/test/proto3)

CollectSourceFiles(
    ${protoc_include}
    private_sources
)

CollectSourceFiles(
    ${protoc_sources}
    private_sources
)

foreach(fil ${private_sources})
  get_filename_component(abs_fil ${fil} ABSOLUTE)
  get_filename_component(fil_we ${fil} NAME_WE)

  list(APPEND srcs_var ${fil_we}.pb.cc)
  list(APPEND hdrs_var ${fil_we}.pb.h)

  add_custom_command(
      OUTPUT ${fil_we}.pb.cc ${fil_we}.pb.h
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out ${output_dir} -I${PROTO_ROOT} ${abs_fil}
      DEPENDS ${abs_fil}
      COMMENT "Running C++ protocol buffer compiler on ${fil}" VERBATIM )
endforeach()

set(target-name out_model)
set(project-name deps)

add_library(${target-name} ${srcs_var} ${hdrs_var})

target_link_libraries(${target-name}
    INTERFACE rendu-dependency-interface
    #    PUBLIC
    )
set_target_properties(${target-name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${target-name} PUBLIC ${hdrs_var})
set_target_properties(${target-name} PROPERTIES FOLDER ${project-name})




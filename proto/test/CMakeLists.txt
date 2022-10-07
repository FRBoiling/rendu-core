#**********************************
#  Created by boil on 2022/8/14.
#**********************************


set(output_dir ${PROTO_ROOT}/out)
set(source_dir ${PROTO_ROOT}/test/unittest)

CollectSourceFiles(
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

set(target-name out_model)
set(project-name deps)

add_library(${target-name} ${srcs_var} ${hdrs_var})

target_link_libraries(${target-name}
    INTERFACE rendu-dependency-interface
    #    PUBLIC
    libprotobuf libprotoc
    )
set_target_properties(${target-name} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${target-name} PUBLIC ${hdrs_var})
set_target_properties(${target-name} PROPERTIES FOLDER ${project-name})




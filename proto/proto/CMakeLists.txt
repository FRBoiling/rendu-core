#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(proto_import_dir ${RENDU_PROJECT_DIR}/dep/protobuf/src)
set(proto_input_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(proto_model_output_dir ${CMAKE_CURRENT_SOURCE_DIR}/..)

rendu_proto_cxx(
    PROJECT
    rendu
    NAME
    cxx
    PROTOC
    tool::proto_generator
    INPUT_DIRS
    ${proto_import_dir}
    ${proto_input_dir}
    OUTPUT_DIR
    ${proto_model_output_dir}/cxx
)

#rendu_proto_csharp(
#    PROJECT
#    rendu
#    NAME
#    csharp
#    PROTOC
#    tool::proto_generator
#    INPUT_DIRS
#    ${proto_import_dir}
#    ${proto_input_dir}
#    OUTPUT_DIR
#    ${proto_model_output_dir}/csharp
#)

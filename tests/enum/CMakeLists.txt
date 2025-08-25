#**********************************
#  Created by boil on 2023/02/19.
#**********************************
get_filename_component(FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

<<<<<<<< HEAD:tests/enum/CMakeLists.txt
set(target_name enum)
rendu_add_test(
========
rendu_add_executable(
>>>>>>>> 3876a41741657cd0f554e14e86a5aa461ee27aef:apps/example/CMakeLists.txt
    DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PROJECT
    ${PROJECT_NAME}
    NAME
<<<<<<<< HEAD:tests/enum/CMakeLists.txt
    ${target_name}
    SETTING
    rendu-core-interface
    DEPS
    dep::magic_enum
    rendu::test
========
    ${FOLDER_NAME}
    LINKOPTS
    rendu-core-interface
    DEPS
    rendu::core
>>>>>>>> 3876a41741657cd0f554e14e86a5aa461ee27aef:apps/example/CMakeLists.txt
    DEFINES
    LINKOPTS
)

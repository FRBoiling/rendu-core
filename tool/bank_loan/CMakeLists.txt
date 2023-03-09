#**********************************
#  Created by boil on 2022/8/14.
#**********************************

set(target_name bank_loan)
rendu_add_executable(
    NAME
    ${target_name}
    SRC_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
    DEPS
    DEFINES
)
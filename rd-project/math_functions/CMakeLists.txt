# first we add the executable that generates the table
add_executable(MakeTable make_table.cxx)

# add the command to generate the source code
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/table.h
  COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/table.h
  DEPENDS MakeTable
  )

# add the main library
add_library(MathFunctions
            mysqrt.cxx
            ${CMAKE_CURRENT_BINARY_DIR}/table.h
            )

# state that anybody linking to us needs to include the current source dir
# to find MathFunctions.h, while we don't.
# state that we depend on Tutorial_BINARY_DIR but consumers don't, as the
# TutorialConfig.h include is an implementation detail
# state that we depend on our binary dir to find Table.h
target_include_directories(MathFunctions
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
          )

# # does this system provide the log and exp functions?
# include(CheckCXXSourceCompiles)
# check_cxx_source_compiles("
#   #include <cmath>
#   int main() {
#     std::log(1.0);
#     return 0;
#   }
# " HAVE_LOG)

# check_cxx_source_compiles("
#   #include <cmath>
#   int main() {
#     std::exp(1.0);
#     return 0;
#   }
# " HAVE_EXP)

# # add compile definitions
# if(HAVE_LOG AND HAVE_EXP)
#   target_compile_definitions(MathFunctions
#                              PRIVATE "HAVE_LOG" "HAVE_EXP")
# endif()

# install rules
install(TARGETS MathFunctions DESTINATION lib)
install(FILES MathFunctions.h DESTINATION include)

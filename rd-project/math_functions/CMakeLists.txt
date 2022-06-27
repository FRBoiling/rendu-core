# first we add the executable that generates the table
add_library(MathFunctions  math_functions.cxx)

# state that anybody linking to us needs to include the current source dir
# to find MathFunctions.h, while we don't.
target_include_directories(MathFunctions
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                           )

# should we use our own math functions
option(USE_MYMATH "Use tutorial provided math implementation" ON)
if(USE_MYMATH)

  target_compile_definitions(MathFunctions PRIVATE "USE_MYMATH")

  # first we add the executable that generates the table
  add_executable(MakeTable make_table.cxx)

  # add the command to generate the source code
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/table.h
    COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/table.h
    DEPENDS MakeTable
    )

  # library that just does sqrt
  add_library(SqrtLibrary STATIC
              mysqrt.cxx
              ${CMAKE_CURRENT_BINARY_DIR}/table.h
              )
  # state that we depend on our binary dir to find Table.h
  target_include_directories(SqrtLibrary PRIVATE
                             ${CMAKE_CURRENT_BINARY_DIR}
                            )

  # state that SqrtLibrary need PIC when the default is shared libraries
  set_target_properties(SqrtLibrary PROPERTIES
                        POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
                        )

  target_link_libraries(MathFunctions PRIVATE SqrtLibrary)
endif()

# define the symbol stating we are using the declspec(dllexport) when
# building on windows
target_compile_definitions(MathFunctions PRIVATE "EXPORTING_MYMATH")

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
set(installable_libs MathFunctions)
if(TARGET SqrtLibrary)
  list(APPEND installable_libs SqrtLibrary)
endif()
install(TARGETS MathFunctions DESTINATION lib)
install(FILES math_functions.h DESTINATION include)

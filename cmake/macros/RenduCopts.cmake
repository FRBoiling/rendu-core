#**********************************
#  Created by boil on 2022/10/19.
#**********************************

list(APPEND RENDU_CLANG_CL_FLAGS
    "/W3"
    "/DNOMINMAX"
    "/DWIN32_LEAN_AND_MEAN"
    "/D_CRT_SECURE_NO_WARNINGS"
    "/D_SCL_SECURE_NO_WARNINGS"
    "/D_ENABLE_EXTENDED_ALIGNED_STORAGE"
)

list(APPEND RENDU_CLANG_CL_TEST_FLAGS
    "-Wno-c99-extensions"
    "-Wno-deprecated-declarations"
    "-Wno-implicit-int-conversion"
    "-Wno-missing-noreturn"
    "-Wno-missing-prototypes"
    "-Wno-missing-variable-declarations"
    "-Wno-null-conversion"
    "-Wno-shadow"
    "-Wno-shift-sign-overflow"
    "-Wno-shorten-64-to-32"
    "-Wno-sign-compare"
    "-Wno-sign-conversion"
    "-Wno-unused-function"
    "-Wno-unused-member-function"
    "-Wno-unused-parameter"
    "-Wno-unused-private-field"
    "-Wno-unused-template"
    "-Wno-used-but-marked-unused"
    "-Wno-zero-as-null-pointer-constant"
    "-Wno-gnu-zero-variadic-macro-arguments"
)

list(APPEND RENDU_GCC_FLAGS
    "-Wall"
    "-Wextra"
    "-Wcast-qual"
    "-Wconversion-null"
    "-Wformat-security"
    "-Wmissing-declarations"
    "-Woverlength-strings"
    "-Wpointer-arith"
    "-Wundef"
    "-Wunused-local-typedefs"
    "-Wunused-result"
    "-Wvarargs"
    "-Wvla"
    "-Wwrite-strings"
    "-DNOMINMAX"
)

list(APPEND RENDU_GCC_TEST_FLAGS
    "-Wno-conversion-null"
    "-Wno-deprecated-declarations"
    "-Wno-missing-declarations"
    "-Wno-sign-compare"
    "-Wno-unused-function"
    "-Wno-unused-parameter"
    "-Wno-unused-private-field"
)

list(APPEND RENDU_LLVM_FLAGS
    "-Wall"
    "-Wextra"
    "-Wcast-qual"
    "-Wconversion"
    "-Wfloat-overflow-conversion"
    "-Wfloat-zero-conversion"
    "-Wfor-loop-analysis"
    "-Wformat-security"
    "-Wgnu-redeclared-enum"
    "-Winfinite-recursion"
    "-Winvalid-constexpr"
    "-Wliteral-conversion"
    "-Wmissing-declarations"
    "-Woverlength-strings"
    "-Wpointer-arith"
    "-Wself-assign"
    "-Wshadow-all"
    "-Wstring-conversion"
    "-Wtautological-overlap-compare"
    "-Wtautological-unsigned-zero-compare"
    "-Wundef"
    "-Wuninitialized"
    "-Wunreachable-code"
    "-Wunused-comparison"
    "-Wunused-local-typedefs"
    "-Wunused-result"
    "-Wvla"
    "-Wwrite-strings"
    "-Wno-float-conversion"
    "-Wno-implicit-float-conversion"
    "-Wno-implicit-int-float-conversion"
    "-Wno-unknown-warning-option"
    "-DNOMINMAX"
)

list(APPEND RENDU_LLVM_TEST_FLAGS
    "-Wno-c99-extensions"
    "-Wno-deprecated-declarations"
    "-Wno-implicit-int-conversion"
    "-Wno-missing-noreturn"
    "-Wno-missing-prototypes"
    "-Wno-missing-variable-declarations"
    "-Wno-null-conversion"
    "-Wno-shadow"
    "-Wno-shift-sign-overflow"
    "-Wno-shorten-64-to-32"
    "-Wno-sign-compare"
    "-Wno-sign-conversion"
    "-Wno-unused-function"
    "-Wno-unused-member-function"
    "-Wno-unused-parameter"
    "-Wno-unused-private-field"
    "-Wno-unused-template"
    "-Wno-used-but-marked-unused"
    "-Wno-zero-as-null-pointer-constant"
    "-Wno-gnu-zero-variadic-macro-arguments"
)

list(APPEND RENDU_MSVC_FLAGS
    "/W3"
    "/DNOMINMAX"
    "/DWIN32_LEAN_AND_MEAN"
    "/D_CRT_SECURE_NO_WARNINGS"
    "/D_SCL_SECURE_NO_WARNINGS"
    "/D_ENABLE_EXTENDED_ALIGNED_STORAGE"
    "/bigobj"
    "/wd4005"
    "/wd4068"
    "/wd4180"
    "/wd4244"
    "/wd4267"
    "/wd4503"
    "/wd4800"
)

list(APPEND RENDU_MSVC_LINKOPTS
    "-ignore:4221"
)

list(APPEND RENDU_MSVC_TEST_FLAGS
    "/wd4018"
    "/wd4101"
    "/wd4503"
    "/wd4996"
    "/DNOMINMAX"
)

list(APPEND RENDU_RANDOM_HWAES_ARM32_FLAGS
    "-mfpu=neon"
)

list(APPEND RENDU_RANDOM_HWAES_ARM64_FLAGS
    "-march=armv8-a+crypto"
)

list(APPEND RENDU_RANDOM_HWAES_MSVC_X64_FLAGS
)

list(APPEND RENDU_RANDOM_HWAES_X64_FLAGS
    "-maes"
    "-msse4.1"
)

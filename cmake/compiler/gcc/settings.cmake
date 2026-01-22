# ====================================================================
# 模块: gcc/settings
# 描述: GCC 编译器相关设置
# 依赖模块:
#   - RenduLogging (日志)
# ====================================================================
function(rendu_setup_gcc_options)
    # ====================================================================
    # 版本检查
    # ====================================================================
    set(RENDU_GCC_EXPECTED_VERSION 11.1.0)

    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS RENDU_GCC_EXPECTED_VERSION)
        rendu_log_fatal("GCC: RenduCore requires version ${RENDU_GCC_EXPECTED_VERSION} to build but found ${CMAKE_CXX_COMPILER_VERSION}")
    else ()
        rendu_log_info("GCC: Minimum version required is ${RENDU_GCC_EXPECTED_VERSION}, found ${CMAKE_CXX_COMPILER_VERSION} - ok!")
    endif ()

    # ====================================================================
    # 基础编译选项
    # ====================================================================
    target_compile_options(rendu-compile-option-interface
            INTERFACE
            -fno-delete-null-pointer-checks)

    # ====================================================================
    # 平台特定选项
    # ====================================================================
    # SSE 指令集只适用于 x86/x64 架构
    if (RENDU_SYSTEM_PROCESSOR MATCHES "x86|amd64")
        if (RENDU_PLATFORM EQUAL 32)
            # 32位系统需要手动开启 SSE2（x64 默认支持）
            target_compile_options(rendu-compile-option-interface
                            INTERFACE
                            -msse2
                            -mfpmath=sse)
        endif ()

        target_compile_definitions(rendu-compile-option-interface
                            INTERFACE
                            HAVE_SSE2
                            __SSE2__)
        rendu_log_info("GCC: SFMT enabled, SSE2 flags forced")
    endif ()

    # ====================================================================
    # 警告选项
    # ====================================================================
    if (RENDU_WITH_WARNINGS)
        target_compile_options(rendu-warning-interface
                INTERFACE
                -W
                -Wall
                -Wextra
                -Winit-self
                -Winvalid-pch
                -Wfatal-errors
                -Woverloaded-virtual
                -Wno-missing-field-initializers # 结构体成员有默认值时该警告无意义
                -Wno-maybe-uninitialized)       # std::optional 场景下该警告易误报
        rendu_log_info("GCC: All warnings enabled")
    endif ()

    # ====================================================================
    # 调试选项
    # ====================================================================
    if (RENDU_WITH_COREDEBUG)
        target_compile_options(rendu-compile-option-interface
                INTERFACE
                -g3)
        message(STATUS "GCC: Debug-flags set (-g3)")
    endif ()

    # ====================================================================
    # Sanitizer 选项
    # ====================================================================
    if (RENDU_ASAN)
        target_compile_options(rendu-compile-option-interface
                INTERFACE
                -fno-omit-frame-pointer
                -fsanitize=address
                -fsanitize-recover=address
                -fsanitize-address-use-after-scope)
        target_link_options(rendu-compile-option-interface
                INTERFACE
                -fno-omit-frame-pointer
                -fsanitize=address
                -fsanitize-recover=address
                -fsanitize-address-use-after-scope)
        rendu_log_info("GCC: Enabled Address Sanitizer")
    endif ()

    # ====================================================================
    # 动态链接选项
    # ====================================================================
    if (BUILD_SHARED_LIBS)
        target_compile_options(rendu-compile-option-interface
                INTERFACE
                -fPIC
                -Wno-attributes)
        target_compile_options(rendu-hidden-symbols-interface
                INTERFACE
                -fvisibility=hidden)
        # -Wl,--no-undefined 可用于链接阶段强制符号完整，但会影响 PCH
        rendu_log_info("GCC: Enabled shared linking")
    endif ()
endfunction()

rendu_setup_gcc_options()
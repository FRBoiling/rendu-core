#
# Copyright 2017 The Abseil Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

find_library(LIBRT rt)

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    atomic_hook
    HDRS
    "internal/atomic_hook.h"
    DEPS
    absl::config
    absl::core_headers
    COPTS
    ${RENDU_DEFAULT_COPTS}
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    errno_saver
    HDRS
    "internal/errno_saver.h"
    DEPS
    absl::config
    COPTS
    ${RENDU_DEFAULT_COPTS}
    )

rendu_cc_library(absl
    NAME
    log_severity
    HDRS
    "log_severity.h"
    SRCS
    "log_severity.cc"
    DEPS
    absl::core_headers
    COPTS
    ${RENDU_DEFAULT_COPTS}
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    raw_logging_internal
    HDRS
    "internal/raw_logging.h"
    SRCS
    "internal/raw_logging.cc"
    DEPS
    absl::atomic_hook
    absl::config
    absl::core_headers
    absl::errno_saver
    absl::log_severity
    COPTS
    ${RENDU_DEFAULT_COPTS}
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    spinlock_wait
    HDRS
    "internal/spinlock_wait.h"
    SRCS
    "internal/spinlock_akaros.inc"
    "internal/spinlock_linux.inc"
    "internal/spinlock_posix.inc"
    "internal/spinlock_wait.cc"
    "internal/spinlock_win32.inc"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::base_internal
    absl::core_headers
    absl::errno_saver
    )

rendu_cc_library(absl
    NAME
    config
    HDRS
    "config.h"
    "options.h"
    "policy_checks.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    PUBLIC
    )

rendu_cc_library(absl
    NAME
    dynamic_annotations
    HDRS
    "dynamic_annotations.h"
    SRCS
    "internal/dynamic_annotations.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    PUBLIC
    )

rendu_cc_library(absl
    NAME
    core_headers
    HDRS
    "attributes.h"
    "const_init.h"
    "macros.h"
    "optimization.h"
    "port.h"
    "thread_annotations.h"
    "internal/thread_annotations.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    PUBLIC
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    malloc_internal
    HDRS
    "internal/direct_mmap.h"
    "internal/low_level_alloc.h"
    SRCS
    "internal/low_level_alloc.cc"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::base
    absl::base_internal
    absl::config
    absl::core_headers
    absl::dynamic_annotations
    absl::raw_logging_internal
    Threads::Threads
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    base_internal
    HDRS
    "internal/hide_ptr.h"
    "internal/identity.h"
    "internal/inline_variable.h"
    "internal/invoke.h"
    "internal/scheduling_mode.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    absl::type_traits
    )

rendu_cc_library(absl
    NAME
    base
    HDRS
    "call_once.h"
    "casts.h"
    "internal/cycleclock.h"
    "internal/cycleclock_config.h"
    "internal/low_level_scheduling.h"
    "internal/per_thread_tls.h"
    "internal/spinlock.h"
    "internal/sysinfo.h"
    "internal/thread_identity.h"
    "internal/tsan_mutex_interface.h"
    "internal/unscaledcycleclock.h"
    "internal/unscaledcycleclock_config.h"
    SRCS
    "internal/cycleclock.cc"
    "internal/spinlock.cc"
    "internal/sysinfo.cc"
    "internal/thread_identity.cc"
    "internal/unscaledcycleclock.cc"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    LINKOPTS
    ${RENDU_DEFAULT_LINKOPTS}
    $<$<BOOL:${LIBRT}>:-lrt>
    $<$<BOOL:${MINGW}>:-ladvapi32>
    DEPS
    absl::atomic_hook
    absl::base_internal
    absl::config
    absl::core_headers
    absl::dynamic_annotations
    absl::log_severity
    absl::raw_logging_internal
    absl::spinlock_wait
    absl::type_traits
    Threads::Threads
    PUBLIC
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    throw_delegate
    HDRS
    "internal/throw_delegate.h"
    SRCS
    "internal/throw_delegate.cc"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    absl::raw_logging_internal
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    exception_testing
    HDRS
    "internal/exception_testing.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    GTest::gtest
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    pretty_function
    HDRS
    "internal/pretty_function.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    exception_safety_testing
    HDRS
    "internal/exception_safety_testing.h"
    SRCS
    "internal/exception_safety_testing.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::config
    absl::pretty_function
    absl::memory
    absl::meta
    absl::strings
    absl::utility
    GTest::gtest
    TESTONLY
    )

rendu_cc_test(absl
    NAME
    absl_exception_safety_testing_test
    SRCS
    "exception_safety_testing_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::exception_safety_testing
    absl::memory
    GTest::gtest_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    atomic_hook_test_helper
    SRCS
    "internal/atomic_hook_test_helper.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::atomic_hook
    absl::core_headers
    TESTONLY
    )

rendu_cc_test(absl
    NAME
    atomic_hook_test
    SRCS
    "internal/atomic_hook_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::atomic_hook_test_helper
    absl::atomic_hook
    absl::core_headers
    GTest::gmock
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    bit_cast_test
    SRCS
    "bit_cast_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::core_headers
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    errno_saver_test
    SRCS
    "internal/errno_saver_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::errno_saver
    absl::strerror
    GTest::gmock
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    throw_delegate_test
    SRCS
    "throw_delegate_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::config
    absl::throw_delegate
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    inline_variable_test
    SRCS
    "internal/inline_variable_testing.h"
    "inline_variable_test.cc"
    "inline_variable_test_a.cc"
    "inline_variable_test_b.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base_internal
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    invoke_test
    SRCS
    "invoke_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base_internal
    absl::memory
    absl::strings
    GTest::gmock
    GTest::gtest_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    spinlock_test_common
    SRCS
    "spinlock_test_common.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::config
    absl::base_internal
    absl::core_headers
    absl::synchronization
    GTest::gtest
    TESTONLY
    )

# On bazel BUILD this target use "alwayslink = 1" which is not implemented here
rendu_cc_test(absl
    NAME
    spinlock_test
    SRCS
    "spinlock_test_common.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::base_internal
    absl::config
    absl::core_headers
    absl::synchronization
    GTest::gtest_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    endian
    HDRS
    "internal/endian.h"
    "internal/unaligned_access.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::base
    absl::config
    absl::core_headers
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    endian_test
    SRCS
    "internal/endian_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::config
    absl::endian
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    config_test
    SRCS
    "config_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::config
    absl::synchronization
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    call_once_test
    SRCS
    "call_once_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::core_headers
    absl::synchronization
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    raw_logging_test
    SRCS
    "raw_logging_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::raw_logging_internal
    absl::strings
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    sysinfo_test
    SRCS
    "internal/sysinfo_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::synchronization
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    low_level_alloc_test
    SRCS
    "internal/low_level_alloc_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::malloc_internal
    absl::node_hash_map
    Threads::Threads
    )

rendu_cc_test(absl
    NAME
    thread_identity_test
    SRCS
    "internal/thread_identity_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    absl::core_headers
    absl::synchronization
    Threads::Threads
    GTest::gtest_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    scoped_set_env
    SRCS
    "internal/scoped_set_env.cc"
    HDRS
    "internal/scoped_set_env.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    absl::raw_logging_internal
    )

rendu_cc_test(absl
    NAME
    scoped_set_env_test
    SRCS
    "internal/scoped_set_env_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::scoped_set_env
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    cmake_thread_test
    SRCS
    "internal/cmake_thread_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::base
    )

rendu_cc_test(absl
    NAME
    log_severity_test
    SRCS
    "log_severity_test.cc"
    DEPS
    absl::flags_internal
    absl::flags_marshalling
    absl::log_severity
    absl::strings
    GTest::gmock
    GTest::gtest_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    strerror
    SRCS
    "internal/strerror.cc"
    HDRS
    "internal/strerror.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    LINKOPTS
    ${RENDU_DEFAULT_LINKOPTS}
    DEPS
    absl::config
    absl::core_headers
    absl::errno_saver
    )

rendu_cc_test(absl
    NAME
    strerror_test
    SRCS
    "internal/strerror_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::strerror
    absl::strings
    GTest::gmock
    GTest::gtest_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    fast_type_id
    HDRS
    "internal/fast_type_id.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    LINKOPTS
    ${RENDU_DEFAULT_LINKOPTS}
    DEPS
    absl::config
    )

rendu_cc_test(absl
    NAME
    fast_type_id_test
    SRCS
    "internal/fast_type_id_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::fast_type_id
    GTest::gtest_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    prefetch
    HDRS
    "internal/prefetch.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    LINKOPTS
    ${RENDU_DEFAULT_LINKOPTS}
    DEPS
    absl::config
    )

rendu_cc_test(absl
    NAME
    prefetch_test
    SRCS
    "internal/prefetch_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::prefetch
    GTest::gtest_main
    )

rendu_cc_test(absl
    NAME
    optimization_test
    SRCS
    "optimization_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::core_headers
    absl::optional
    GTest::gtest_main
    )

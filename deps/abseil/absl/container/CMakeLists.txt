#
# Copyright 2017 The Abseil Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

rendu_cc_library(absl
    NAME
    btree
    HDRS
    "btree_map.h"
    "btree_set.h"
    "internal/btree.h"
    "internal/btree_container.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    LINKOPTS
    ${RENDU_DEFAULT_LINKOPTS}
    DEPS
    absl::container_common
    absl::common_policy_traits
    absl::compare
    absl::compressed_tuple
    absl::container_memory
    absl::cord
    absl::core_headers
    absl::layout
    absl::memory
    absl::raw_logging_internal
    absl::strings
    absl::throw_delegate
    absl::type_traits
    absl::utility
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    btree_test_common
    hdrs
    "btree_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    LINKOPTS
    ${RENDU_DEFAULT_LINKOPTS}
    DEPS
    absl::btree
    absl::cord
    absl::flat_hash_set
    absl::strings
    absl::time
    TESTONLY
    )

rendu_cc_test(absl
    NAME
    btree_test
    SRCS
    "btree_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    LINKOPTS
    ${RENDU_DEFAULT_LINKOPTS}
    DEPS
    absl::algorithm_container
    absl::btree
    absl::btree_test_common
    absl::compare
    absl::core_headers
    absl::counting_allocator
    absl::flags
    absl::hash_testing
    absl::random_random
    absl::raw_logging_internal
    absl::strings
    absl::test_instance_tracker
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    compressed_tuple
    HDRS
    "internal/compressed_tuple.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::utility
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    compressed_tuple_test
    SRCS
    "internal/compressed_tuple_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::any
    absl::compressed_tuple
    absl::memory
    absl::optional
    absl::test_instance_tracker
    absl::utility
    GTest::gmock_main
    )

rendu_cc_library(absl
    NAME
    fixed_array
    HDRS
    "fixed_array.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::compressed_tuple
    absl::algorithm
    absl::config
    absl::core_headers
    absl::dynamic_annotations
    absl::throw_delegate
    absl::memory
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    fixed_array_test
    SRCS
    "fixed_array_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::fixed_array
    absl::counting_allocator
    absl::config
    absl::exception_testing
    absl::hash_testing
    absl::memory
    GTest::gmock_main
    )

rendu_cc_test(absl
    NAME
    fixed_array_exception_safety_test
    SRCS
    "fixed_array_exception_safety_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::fixed_array
    absl::config
    absl::exception_safety_testing
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    inlined_vector_internal
    HDRS
    "internal/inlined_vector.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::compressed_tuple
    absl::core_headers
    absl::memory
    absl::span
    absl::type_traits
    PUBLIC
    )

rendu_cc_library(absl
    NAME
    inlined_vector
    HDRS
    "inlined_vector.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::algorithm
    absl::core_headers
    absl::inlined_vector_internal
    absl::throw_delegate
    absl::memory
    absl::type_traits
    PUBLIC
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    counting_allocator
    HDRS
    "internal/counting_allocator.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    )

rendu_cc_test(absl
    NAME
    inlined_vector_test
    SRCS
    "inlined_vector_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::counting_allocator
    absl::inlined_vector
    absl::test_instance_tracker
    absl::config
    absl::core_headers
    absl::exception_testing
    absl::hash_testing
    absl::memory
    absl::raw_logging_internal
    absl::strings
    GTest::gmock_main
    )

rendu_cc_test(absl
    NAME
    inlined_vector_exception_safety_test
    SRCS
    "inlined_vector_exception_safety_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::inlined_vector
    absl::config
    absl::exception_safety_testing
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    test_instance_tracker
    HDRS
    "internal/test_instance_tracker.h"
    SRCS
    "internal/test_instance_tracker.cc"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::compare
    TESTONLY
    )

rendu_cc_test(absl
    NAME
    test_instance_tracker_test
    SRCS
    "internal/test_instance_tracker_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::test_instance_tracker
    GTest::gmock_main
    )

rendu_cc_library(absl
    NAME
    flat_hash_map
    HDRS
    "flat_hash_map.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::container_memory
    absl::core_headers
    absl::hash_function_defaults
    absl::raw_hash_map
    absl::algorithm_container
    absl::memory
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    flat_hash_map_test
    SRCS
    "flat_hash_map_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::flat_hash_map
    absl::hash_generator_testing
    absl::unordered_map_constructor_test
    absl::unordered_map_lookup_test
    absl::unordered_map_members_test
    absl::unordered_map_modifiers_test
    absl::any
    absl::raw_logging_internal
    GTest::gmock_main
    )

rendu_cc_library(absl
    NAME
    flat_hash_set
    HDRS
    "flat_hash_set.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::container_memory
    absl::hash_function_defaults
    absl::raw_hash_set
    absl::algorithm_container
    absl::core_headers
    absl::memory
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    flat_hash_set_test
    SRCS
    "flat_hash_set_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    "-DUNORDERED_SET_CXX17"
    DEPS
    absl::flat_hash_set
    absl::hash_generator_testing
    absl::unordered_set_constructor_test
    absl::unordered_set_lookup_test
    absl::unordered_set_members_test
    absl::unordered_set_modifiers_test
    absl::memory
    absl::raw_logging_internal
    absl::strings
    GTest::gmock_main
    )

rendu_cc_library(absl
    NAME
    node_hash_map
    HDRS
    "node_hash_map.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::container_memory
    absl::core_headers
    absl::hash_function_defaults
    absl::node_slot_policy
    absl::raw_hash_map
    absl::algorithm_container
    absl::memory
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    node_hash_map_test
    SRCS
    "node_hash_map_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_generator_testing
    absl::node_hash_map
    absl::tracked
    absl::unordered_map_constructor_test
    absl::unordered_map_lookup_test
    absl::unordered_map_members_test
    absl::unordered_map_modifiers_test
    GTest::gmock_main
    )

rendu_cc_library(absl
    NAME
    node_hash_set
    HDRS
    "node_hash_set.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::core_headers
    absl::hash_function_defaults
    absl::node_slot_policy
    absl::raw_hash_set
    absl::algorithm_container
    absl::memory
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    node_hash_set_test
    SRCS
    "node_hash_set_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    "-DUNORDERED_SET_CXX17"
    DEPS
    absl::hash_generator_testing
    absl::node_hash_set
    absl::unordered_set_constructor_test
    absl::unordered_set_lookup_test
    absl::unordered_set_members_test
    absl::unordered_set_modifiers_test
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    container_memory
    HDRS
    "internal/container_memory.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    absl::memory
    absl::type_traits
    absl::utility
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    container_memory_test
    SRCS
    "internal/container_memory_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::container_memory
    absl::strings
    absl::test_instance_tracker
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    hash_function_defaults
    HDRS
    "internal/hash_function_defaults.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    absl::cord
    absl::hash
    absl::strings
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    hash_function_defaults_test
    SRCS
    "internal/hash_function_defaults_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::cord
    absl::cord_test_helpers
    absl::hash_function_defaults
    absl::hash
    absl::random_random
    absl::strings
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    hash_generator_testing
    HDRS
    "internal/hash_generator_testing.h"
    SRCS
    "internal/hash_generator_testing.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_policy_testing
    absl::memory
    absl::meta
    absl::strings
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    hash_policy_testing
    HDRS
    "internal/hash_policy_testing.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash
    absl::strings
    TESTONLY
    )

rendu_cc_test(absl
    NAME
    hash_policy_testing_test
    SRCS
    "internal/hash_policy_testing_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_policy_testing
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    hash_policy_traits
    HDRS
    "internal/hash_policy_traits.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::common_policy_traits
    absl::meta
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    hash_policy_traits_test
    SRCS
    "internal/hash_policy_traits_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_policy_traits
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    common_policy_traits
    HDRS
    "internal/common_policy_traits.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::meta
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    common_policy_traits_test
    SRCS
    "internal/common_policy_traits_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::common_policy_traits
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    hashtablez_sampler
    HDRS
    "internal/hashtablez_sampler.h"
    SRCS
    "internal/hashtablez_sampler.cc"
    "internal/hashtablez_sampler_force_weak_definition.cc"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::base
    absl::config
    absl::exponential_biased
    absl::sample_recorder
    absl::synchronization
    )

rendu_cc_test(absl
    NAME
    hashtablez_sampler_test
    SRCS
    "internal/hashtablez_sampler_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::config
    absl::hashtablez_sampler
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    hashtable_debug
    HDRS
    "internal/hashtable_debug.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::hashtable_debug_hooks
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    hashtable_debug_hooks
    HDRS
    "internal/hashtable_debug_hooks.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    PUBLIC
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    node_slot_policy
    HDRS
    "internal/node_slot_policy.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    node_slot_policy_test
    SRCS
    "internal/node_slot_policy_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_policy_traits
    absl::node_slot_policy
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    raw_hash_map
    HDRS
    "internal/raw_hash_map.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::container_memory
    absl::raw_hash_set
    absl::throw_delegate
    PUBLIC
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    container_common
    HDRS
    "internal/common.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::type_traits
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    raw_hash_set
    HDRS
    "internal/raw_hash_set.h"
    SRCS
    "internal/raw_hash_set.cc"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::bits
    absl::compressed_tuple
    absl::config
    absl::container_common
    absl::container_memory
    absl::core_headers
    absl::endian
    absl::hash_policy_traits
    absl::hashtable_debug_hooks
    absl::memory
    absl::meta
    absl::optional
    absl::prefetch
    absl::utility
    absl::hashtablez_sampler
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    raw_hash_set_test
    SRCS
    "internal/raw_hash_set_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::container_memory
    absl::flat_hash_map
    absl::flat_hash_set
    absl::hash_function_defaults
    absl::hash_policy_testing
    absl::hashtable_debug
    absl::raw_hash_set
    absl::base
    absl::config
    absl::log
    absl::core_headers
    absl::prefetch
    absl::raw_logging_internal
    absl::strings
    GTest::gmock_main
    )

rendu_cc_test(absl
    NAME
    raw_hash_set_allocator_test
    SRCS
    "internal/raw_hash_set_allocator_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::raw_hash_set
    absl::tracked
    absl::core_headers
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    layout
    HDRS
    "internal/layout.h"
    COPTS
    ${RENDU_DEFAULT_COPTS}
    DEPS
    absl::config
    absl::core_headers
    absl::meta
    absl::strings
    absl::span
    absl::utility
    PUBLIC
    )

rendu_cc_test(absl
    NAME
    layout_test
    SRCS
    "internal/layout_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::layout
    absl::config
    absl::core_headers
    absl::raw_logging_internal
    absl::span
    GTest::gmock_main
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    tracked
    HDRS
    "internal/tracked.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::config
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_map_constructor_test
    HDRS
    "internal/unordered_map_constructor_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_generator_testing
    absl::hash_policy_testing
    GTest::gmock
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_map_lookup_test
    HDRS
    "internal/unordered_map_lookup_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_generator_testing
    absl::hash_policy_testing
    GTest::gmock
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_map_members_test
    HDRS
    "internal/unordered_map_members_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::type_traits
    GTest::gmock
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_map_modifiers_test
    HDRS
    "internal/unordered_map_modifiers_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_generator_testing
    absl::hash_policy_testing
    GTest::gmock
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_set_constructor_test
    HDRS
    "internal/unordered_set_constructor_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_generator_testing
    absl::hash_policy_testing
    GTest::gmock
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_set_lookup_test
    HDRS
    "internal/unordered_set_lookup_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_generator_testing
    absl::hash_policy_testing
    GTest::gmock
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_set_members_test
    HDRS
    "internal/unordered_set_members_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::type_traits
    GTest::gmock
    TESTONLY
    )

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
    NAME
    unordered_set_modifiers_test
    HDRS
    "internal/unordered_set_modifiers_test.h"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::hash_generator_testing
    absl::hash_policy_testing
    GTest::gmock
    TESTONLY
    )

rendu_cc_test(absl
    NAME
    unordered_set_test
    SRCS
    "internal/unordered_set_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::unordered_set_constructor_test
    absl::unordered_set_lookup_test
    absl::unordered_set_members_test
    absl::unordered_set_modifiers_test
    GTest::gmock_main
    )

rendu_cc_test(absl
    NAME
    unordered_map_test
    SRCS
    "internal/unordered_map_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::unordered_map_constructor_test
    absl::unordered_map_lookup_test
    absl::unordered_map_members_test
    absl::unordered_map_modifiers_test
    GTest::gmock_main
    )

rendu_cc_test(absl
    NAME
    sample_element_size_test
    SRCS
    "sample_element_size_test.cc"
    COPTS
    ${RENDU_TEST_COPTS}
    DEPS
    absl::flat_hash_map
    absl::flat_hash_set
    absl::node_hash_map
    absl::node_hash_set
    GTest::gmock_main
    )

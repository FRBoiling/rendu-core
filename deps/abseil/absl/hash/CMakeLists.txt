#
# Copyright 2018 The Abseil Authors.
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
    hash
  HDRS
    "hash.h"
  SRCS
    "internal/hash.cc"
    "internal/hash.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::city
    absl::config
    absl::core_headers
    absl::endian
    absl::fixed_array
    absl::function_ref
    absl::meta
    absl::int128
    absl::strings
    absl::optional
    absl::variant
    absl::utility
    absl::low_level_hash
  PUBLIC
)

rendu_cc_library(absl
  NAME
    hash_testing
  HDRS
    "hash_testing.h"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::spy_hash_state
    absl::meta
    absl::strings
    absl::variant
    GTest::gmock
  TESTONLY
  PUBLIC
)

rendu_cc_test(absl
  NAME
    hash_test
  SRCS
    "hash_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::cord_test_helpers
    absl::hash
    absl::hash_testing
    absl::core_headers
    absl::btree
    absl::flat_hash_map
    absl::flat_hash_set
    absl::node_hash_map
    absl::node_hash_set
    absl::spy_hash_state
    absl::meta
    absl::int128
    GTest::gmock_main
)

# Internal-only target, do not depend on directly.
#
# Note: Even though external code should not depend on this target
# directly, it must be marked PUBLIC since it is a dependency of
# hash_testing.
rendu_cc_library(absl
  NAME
    spy_hash_state
  HDRS
    "internal/spy_hash_state.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::hash
    absl::strings
    absl::str_format
  TESTONLY
  PUBLIC
)

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
  NAME
    city
  HDRS
    "internal/city.h"
  SRCS
    "internal/city.cc"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::config
    absl::core_headers
    absl::endian
)

rendu_cc_test(absl
  NAME
    city_test
  SRCS
    "internal/city_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::city
    GTest::gmock_main
)

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
  NAME
    low_level_hash
  HDRS
    "internal/low_level_hash.h"
  SRCS
    "internal/low_level_hash.cc"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::bits
    absl::config
    absl::endian
    absl::int128
)

rendu_cc_test(absl
  NAME
    low_level_hash_test
  SRCS
    "internal/low_level_hash_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::low_level_hash
    absl::strings
    GTest::gmock_main
)

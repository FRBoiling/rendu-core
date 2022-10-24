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
    any
  HDRS
    "any.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::bad_any_cast
    absl::config
    absl::core_headers
    absl::fast_type_id
    absl::type_traits
    absl::utility
  PUBLIC
)

rendu_cc_library(absl
  NAME
    bad_any_cast
  HDRS
   "bad_any_cast.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::bad_any_cast_impl
    absl::config
  PUBLIC
)

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
  NAME
    bad_any_cast_impl
  SRCS
   "bad_any_cast.h"
   "bad_any_cast.cc"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::config
    absl::raw_logging_internal
)

rendu_cc_library(absl
  NAME
    span
  HDRS
    "span.h"
  SRCS
    "internal/span.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::algorithm
    absl::core_headers
    absl::throw_delegate
    absl::type_traits
  PUBLIC
)

rendu_cc_library(absl
  NAME
    optional
  HDRS
    "optional.h"
  SRCS
    "internal/optional.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::bad_optional_access
    absl::base_internal
    absl::config
    absl::core_headers
    absl::memory
    absl::type_traits
    absl::utility
  PUBLIC
)

rendu_cc_library(absl
  NAME
    bad_optional_access
  HDRS
    "bad_optional_access.h"
  SRCS
    "bad_optional_access.cc"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::config
    absl::raw_logging_internal
  PUBLIC
)

rendu_cc_library(absl
  NAME
    bad_variant_access
  HDRS
    "bad_variant_access.h"
  SRCS
    "bad_variant_access.cc"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::config
    absl::raw_logging_internal
  PUBLIC
)

# Internal-only target, do not depend on directly.
rendu_cc_library(absl
  NAME
    conformance_testing
  HDRS
    "internal/conformance_aliases.h"
    "internal/conformance_archetype.h"
    "internal/conformance_profile.h"
    "internal/conformance_testing.h"
    "internal/conformance_testing_helpers.h"
    "internal/parentheses.h"
    "internal/transform_args.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::algorithm
    absl::debugging
    absl::type_traits
    absl::strings
    absl::utility
    GTest::gmock_main
  TESTONLY
)

rendu_cc_library(absl
  NAME
    variant
  HDRS
    "variant.h"
  SRCS
    "internal/variant.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::bad_variant_access
    absl::base_internal
    absl::config
    absl::core_headers
    absl::type_traits
    absl::utility
  PUBLIC
)

rendu_cc_test(absl
  NAME
    variant_test
  SRCS
    "variant_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::variant
    absl::config
    absl::core_headers
    absl::memory
    absl::type_traits
    absl::strings
    GTest::gmock_main
)

rendu_cc_library(absl
  NAME
    compare
  HDRS
    "compare.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::core_headers
    absl::type_traits
  PUBLIC
)

rendu_cc_test(absl
  NAME
    compare_test
  SRCS
    "compare_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::base
    absl::compare
    GTest::gmock_main
)

rendu_cc_test(absl
  NAME
    variant_exception_safety_test
  SRCS
    "variant_exception_safety_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::variant
    absl::config
    absl::exception_safety_testing
    absl::memory
    GTest::gmock_main
)

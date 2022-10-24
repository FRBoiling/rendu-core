#
# Copyright 2020 The Abseil Authors.
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
    status
  HDRS
    "status.h"
  SRCS
    "internal/status_internal.h"
    "status.cc"
    "status_payload_printer.h"
    "status_payload_printer.cc"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::atomic_hook
    absl::config
    absl::cord
    absl::core_headers
    absl::function_ref
    absl::inlined_vector
    absl::optional
    absl::raw_logging_internal
    absl::stacktrace
    absl::str_format
    absl::strerror
    absl::strings
    absl::symbolize
  PUBLIC
)

rendu_cc_test(absl
  NAME
    status_test
  SRCS
   "status_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::status
    absl::strings
    GTest::gmock_main
)

rendu_cc_library(absl
  NAME
    statusor
  HDRS
    "statusor.h"
  SRCS
    "statusor.cc"
    "internal/statusor_internal.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::base
    absl::status
    absl::core_headers
    absl::raw_logging_internal
    absl::type_traits
    absl::strings
    absl::utility
    absl::variant
  PUBLIC
)

rendu_cc_test(absl
  NAME
    statusor_test
  SRCS
   "statusor_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::status
    absl::statusor
    GTest::gmock_main
)

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
    utility
  HDRS
    "utility.h"
  COPTS
    ${RENDU_DEFAULT_COPTS}
  DEPS
    absl::base_internal
    absl::config
    absl::type_traits
  PUBLIC
)

rendu_cc_test(absl
  NAME
    utility_test
  SRCS
    "utility_test.cc"
  COPTS
    ${RENDU_TEST_COPTS}
  DEPS
    absl::utility
    absl::core_headers
    absl::memory
    absl::strings
    GTest::gmock_main
)

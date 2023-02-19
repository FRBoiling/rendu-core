/*
* Created by boil on 2023/2/14.
*/
#include <test/rdtest.h>
#include <common/utils/string_util.h>

namespace {

RD_TEST(StringTest, TruncSubstr) {
  const std::string_view hi("hi");
  RD_EXPECT_EQ("", rendu::ClippedSubstr(hi, 0, 0));
  RD_EXPECT_EQ("h", rendu::ClippedSubstr(hi, 0, 1));
  RD_EXPECT_EQ("hi", rendu::ClippedSubstr(hi, 0));
  RD_EXPECT_EQ("i", rendu::ClippedSubstr(hi, 1));
  RD_EXPECT_EQ("", rendu::ClippedSubstr(hi, 2));
  RD_EXPECT_EQ("", rendu::ClippedSubstr(hi, 3));  // truncation
  RD_EXPECT_EQ("", rendu::ClippedSubstr(hi, 3, 2));  // truncation
}

RD_TEST(StringTest, NullSafeStringView) {
  {
    std::string_view s = rendu::NullSafeStringView(nullptr);
    RD_EXPECT_EQ(nullptr, s.data());
    RD_EXPECT_EQ(0u, s.size());
    RD_EXPECT_EQ(std::string_view(), s);
  }
  {
    static const char kHi[] = "hi";
    std::string_view s = rendu::NullSafeStringView(kHi);
    RD_EXPECT_EQ(kHi, s.data());
    RD_EXPECT_EQ(strlen(kHi), s.size());
    RD_EXPECT_EQ(std::string_view("hi"), s);
  }
}

RD_TEST(StringTest, ConstexprNullSafeStringView) {
  {
    constexpr std::string_view s = rendu::NullSafeStringView(nullptr);
    RD_EXPECT_EQ(nullptr, s.data());
    RD_EXPECT_EQ(0u, s.size());
    RD_EXPECT_EQ(std::string_view(), s);
  }
#if !defined(_MSC_VER) || _MSC_VER >= 1910
// MSVC 2017+ is required for good constexpr string_view support.
// See the implementation of `std::string_view::StrlenInternal()`.
  {
    static constexpr char kHi[] = "hi";
    std::string_view s = rendu::NullSafeStringView(kHi);
    RD_EXPECT_EQ(kHi, s.data());
    RD_EXPECT_EQ(strlen(kHi), s.size());
    RD_EXPECT_EQ(std::string_view("hi"), s);
  }
  {
    constexpr std::string_view s = rendu::NullSafeStringView("hello");
    RD_EXPECT_EQ(s.size(), 5u);
    RD_EXPECT_EQ("hello", s);
  }
#endif
}
}
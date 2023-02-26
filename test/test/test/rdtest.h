/*
* Created by boil on 2023/2/19.
*/

#ifndef RENDU_TEST_RD_TEST_H_
#define RENDU_TEST_RD_TEST_H_

#include <gtest/gtest.h>

#define RD_TEST(test_suite_name, test_name) TEST(test_suite_name, test_name)
#define RD_TEST_F(test_fixture, test_name) TEST_F(test_fixture, test_name)
#define RD_TYPED_TEST(CaseName, TestName)  TYPED_TEST(CaseName, TestName)
#define RD_TYPED_TEST_SUITE(CaseName, Types, ...) TYPED_TEST_SUITE(CaseName, Types, __VA_ARGS__)

#ifdef NDEBUG
#    define RD_DEBUG_TEST(Case, Test) RD_TEST(Case, DISABLED_##Test)
#    define RD_DEBUG_TEST_F(Case, Test) RD_TEST_F(Case, DISABLED_##Test)
#    define RD_DEBUG_TYPED_TEST(Case, Test) TYPED_TEST(Case, DISABLED_##Test)
#else
#    define RD_DEBUG_TEST(Case, Test) RD_TEST(Case, Test)
#    define RD_DEBUG_TEST_F(Case, Test) RD_TEST_F(Case, Test)
#    define RD_DEBUG_TYPED_TEST(Case, Test) RD_TYPED_TEST(Case, Test)
#endif

#define RD_EXPECT_NE(val1, val2)  EXPECT_NE(val1, val2)
#define RD_EXPECT_EQ(val1, val2)  EXPECT_EQ(val1, val2)
#define RD_EXPECT_LE(val1, val2)  EXPECT_LE(val1, val2)
#define RD_EXPECT_LT(val1, val2)  EXPECT_LT(val1, val2)
#define RD_EXPECT_GE(val1, val2)  EXPECT_GE(val1, val2)
#define RD_EXPECT_GT(val1, val2)  EXPECT_GT(val1, val2)
#define RD_EXPECT_TRUE(condition) EXPECT_TRUE(condition)
#define RD_EXPECT_FALSE(condition) EXPECT_FALSE(condition)

#define RD_ASSERT_NE(val1, val2) ASSERT_NE(val1, val2)
#define RD_ASSERT_EQ(val1, val2) ASSERT_EQ(val1, val2)
#define RD_ASSERT_NE(val1, val2) ASSERT_NE(val1, val2)
#define RD_ASSERT_LE(val1, val2) ASSERT_LE(val1, val2)
#define RD_ASSERT_LT(val1, val2) ASSERT_LT(val1, val2)
#define RD_ASSERT_GE(val1, val2) ASSERT_GE(val1, val2)
#define RD_ASSERT_GT(val1, val2) ASSERT_GT(val1, val2)
#define RD_ASSERT_FALSE(condition) ASSERT_FALSE(condition)
#define RD_ASSERT_TRUE(condition) ASSERT_TRUE(condition)

#define RD_ASSERT_NO_FATAL_FAILURE(statement) ASSERT_NO_FATAL_FAILURE(statement)
#define RD_EXPECT_NO_FATAL_FAILURE(statement) EXPECT_NO_FATAL_FAILURE(statement)

#define RD_ASSERT_DEATH(statement, matcher) ASSERT_DEATH(statement, matcher)
#define RD_EXPECT_DEATH(statement, matcher) EXPECT_DEATH(statement, matcher)

#define RD_EXPECT_THROW(statement, expected_exception) EXPECT_THROW(statement, expected_exception)
#define RD_EXPECT_NO_THROW(statement) EXPECT_NO_THROW(statement)
#define RD_EXPECT_ANY_THROW(statement) EXPECT_ANY_THROW(statement)

#define RD_ASSERT_THROW(statement, expected_exception) ASSERT_THROW(statement, expected_exception)
#define RD_ASSERT_NO_THROW(statement) ASSERT_NO_THROW(statement)
#define RD_ASSERT_ANY_THROW(statement) ASSERT_ANY_THROW(statement)

#define RD_EXPECT_STREQ(s1, s2) EXPECT_STREQ(s1, s2)
#define RD_EXPECT_STRNE(s1, s2) EXPECT_STRNE(s1, s2)
#define RD_EXPECT_STRCASEEQ(s1, s2) EXPECT_STRCASEEQ(s1, s2)
#define RD_EXPECT_STRCASENE(s1, s2) EXPECT_STRCASENE(s1, s2)

#define RD_ASSERT_STREQ(s1, s2) ASSERT_STREQ(s1, s2)
#define RD_ASSERT_STRNE(s1, s2) ASSERT_STRNE(s1, s2)
#define RD_ASSERT_STRCASEEQ(s1, s2) ASSERT_STRCASEEQ(s1, s2)
#define RD_ASSERT_STRCASENE(s1, s2) ASSERT_STRCASENE(s1, s2)



#endif //RENDU_TEST_RD_TEST_H_

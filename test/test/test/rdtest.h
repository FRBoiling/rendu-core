/*
* Created by boil on 2023/2/19.
*/

#ifndef RENDU_TEST_RENDUTEST_H_
#define RENDU_TEST_RENDUTEST_H_

#include <gtest/gtest.h>

#ifdef NDEBUG
#    define RENDU_DEBUG_TEST(Case, Test) RENDU_TEST(Case, DISABLED_##Test)
#    define RENDU_DEBUG_TEST_F(Case, Test) RENDU_TEST_F(Case, DISABLED_##Test)
#    define RENDU_DEBUG_TYPED_TEST(Case, Test) TYPED_TEST(Case, DISABLED_##Test)
#else
#    define RENDU_DEBUG_TEST(Case, Test) RENDU_TEST(Case, Test)
#    define RENDU_DEBUG_TEST_F(Case, Test) RENDU_TEST_F(Case, Test)
#    define RENDU_DEBUG_TYPED_TEST(Case, Test) TYPED_TEST(Case, Test)
#endif

#define RENDU_TEST(test_suite_name, test_name) TEST(test_suite_name, test_name)
#define RENDU_TEST_F(test_fixture, test_name) TEST_F(test_fixture, test_name)

#define RENDU_EXPECT_NE(val1, val2)  EXPECT_NE(val1, val2)
#define RENDU_EXPECT_EQ(val1, val2)  EXPECT_EQ(val1, val2)
#define RENDU_EXPECT_LE(val1, val2)  EXPECT_LE(val1, val2)
#define RENDU_EXPECT_LT(val1, val2)  EXPECT_LT(val1, val2)
#define RENDU_EXPECT_GE(val1, val2)  EXPECT_GE(val1, val2)
#define RENDU_EXPECT_GT(val1, val2)  EXPECT_GT(val1, val2)
#define RENDU_EXPECT_TRUE(condition) EXPECT_TRUE(condition)
#define RENDU_EXPECT_FALSE(condition) EXPECT_FALSE(condition)

#define RENDU_ASSERT_NE(val1, val2) ASSERT_NE(val1, val2)
#define RENDU_ASSERT_EQ(val1, val2) ASSERT_EQ(val1, val2)
#define RENDU_ASSERT_NE(val1, val2) ASSERT_NE(val1, val2)
#define RENDU_ASSERT_LE(val1, val2) ASSERT_LE(val1, val2)
#define RENDU_ASSERT_LT(val1, val2) ASSERT_LT(val1, val2)
#define RENDU_ASSERT_GE(val1, val2) ASSERT_GE(val1, val2)
#define RENDU_ASSERT_GT(val1, val2) ASSERT_GT(val1, val2)
#define RENDU_ASSERT_FALSE(condition) ASSERT_FALSE(condition)
#define RENDU_ASSERT_TRUE(condition) ASSERT_TRUE(condition)

#define RENDU_ASSERT_NO_FATAL_FAILURE(statement) ASSERT_NO_FATAL_FAILURE(statement)
#define RENDU_EXPECT_NO_FATAL_FAILURE(statement) EXPECT_NO_FATAL_FAILURE(statement)

#define RENDU_ASSERT_DEATH(statement, matcher) ASSERT_DEATH(statement, matcher)
#define RENDU_EXPECT_DEATH(statement, matcher) EXPECT_DEATH(statement, matcher)

#define RENDU_EXPECT_THROW(statement, expected_exception) EXPECT_THROW(statement, expected_exception)
#define RENDU_EXPECT_NO_THROW(statement) EXPECT_NO_THROW(statement)
#define RENDU_EXPECT_ANY_THROW(statement) EXPECT_ANY_THROW(statement)

#define RENDU_ASSERT_THROW(statement, expected_exception) ASSERT_THROW(statement, expected_exception)
#define RENDU_ASSERT_NO_THROW(statement) ASSERT_NO_THROW(statement)
#define RENDU_ASSERT_ANY_THROW(statement) ASSERT_ANY_THROW(statement)

#endif //RENDU_TEST_RENDUTEST_H_

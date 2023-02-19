/*
* Created by boil on 2023/2/18.
*/

#ifndef TEST_RENDU_DEFINE_H_
#define TEST_RENDU_DEFINE_H_
namespace test {

#ifdef NDEBUG
#    define RD_DEBUG_TEST(Case, Test) RD_TEST(Case, DISABLED_##Test)
#    define RD_DEBUG_TEST_F(Case, Test) RD_TEST_F(Case, DISABLED_##Test)
#    define RD_DEBUG_TYPED_TEST(Case, Test) TYPED_TEST(Case, DISABLED_##Test)
#else
#    define RD_DEBUG_TEST(Case, Test) RD_TEST(Case, Test)
#    define RD_DEBUG_TEST_F(Case, Test) RD_TEST_F(Case, Test)
#    define RD_DEBUG_TYPED_TEST(Case, Test) TYPED_TEST(Case, Test)
#endif



} // namespace test
#endif //TEST_RENDU_DEFINE_H_

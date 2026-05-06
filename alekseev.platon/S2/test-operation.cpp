#include <boost/test/unit_test.hpp>

#include <climits>
#include <stdexcept>

#include "operation.hpp"

BOOST_AUTO_TEST_CASE(precedence_all)
{
  BOOST_TEST(alekseev::precedence('#') > alekseev::precedence('*'));
  BOOST_TEST(alekseev::precedence('*') > alekseev::precedence('+'));

  BOOST_TEST(alekseev::precedence('*') == alekseev::precedence('/'));
  BOOST_TEST(alekseev::precedence('/') == alekseev::precedence('%'));
  BOOST_TEST(alekseev::precedence('+') == alekseev::precedence('-'));
}

BOOST_AUTO_TEST_CASE(apply_binary_simple)
{
  BOOST_TEST(alekseev::applyBinary('+', 1, 2) == 3);
  BOOST_TEST(alekseev::applyBinary('-', 5, 3) == 2);
  BOOST_TEST(alekseev::applyBinary('*', 4, 3) == 12);
  BOOST_TEST(alekseev::applyBinary('/', 9, 3) == 3);
  BOOST_TEST(alekseev::applyBinary('%', 10, 3) == 1);
}

BOOST_AUTO_TEST_CASE(apply_unary_reverse_examples)
{
  BOOST_TEST(alekseev::applyUnary('#', 123) == 321);
  BOOST_TEST(alekseev::applyUnary('#', 1200) == 21);
  BOOST_TEST(alekseev::applyUnary('#', -305) == -503);
}

BOOST_AUTO_TEST_CASE(domain_errors)
{
  BOOST_CHECK_THROW(alekseev::applyBinary('/', 1, 0), std::domain_error);
  BOOST_CHECK_THROW(alekseev::applyBinary('%', 1, 0), std::domain_error);
}

BOOST_AUTO_TEST_CASE(overflow_binary)
{
  BOOST_CHECK_THROW(alekseev::applyBinary('+', LLONG_MAX, 1), std::overflow_error);
  BOOST_CHECK_THROW(alekseev::applyBinary('*', LLONG_MAX, 2), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(overflow_reverse)
{
  const long long x = 8085774586302733229LL;
  BOOST_CHECK_THROW(alekseev::reverseNumber(x), std::overflow_error);
  BOOST_CHECK_THROW(alekseev::applyUnary('#', x), std::overflow_error);
}

#include <boost/test/unit_test.hpp>

#include "name_utils.hpp"

BOOST_AUTO_TEST_CASE(valid_names)
{
  BOOST_CHECK(alekseev::isValidName("p1"));
  BOOST_CHECK(alekseev::isValidName("Project1"));
  BOOST_CHECK(alekseev::isValidName("A"));
  BOOST_CHECK(alekseev::isValidName("build123"));
}

BOOST_AUTO_TEST_CASE(invalid_names)
{
  BOOST_CHECK(!alekseev::isValidName(""));
  BOOST_CHECK(!alekseev::isValidName("1abc"));
  BOOST_CHECK(!alekseev::isValidName("_abc"));
  BOOST_CHECK(!alekseev::isValidName("abc-def"));
  BOOST_CHECK(!alekseev::isValidName("abc.def"));
  BOOST_CHECK(!alekseev::isValidName("абв"));
  BOOST_CHECK(!alekseev::isValidName("abc def"));
}
